"""
PFRLの公式サンプルをコピー
"""

import argparse
import json
import os

import numpy as np
import torch
from torch.utils.tensorboard import SummaryWriter

import pfrl
from pfrl import agents, experiments, explorers
from pfrl import nn as pnn
from pfrl import replay_buffers, utils
from pfrl.q_functions import DistributionalDuelingDQN
from pfrl.wrappers import atari_wrappers
from map_env_cy import MapRootEnv
import torch.nn as nn
import datetime
import parse
import logging


class TBLogger:
    STEP_F = "outdir:{} step:{:d} episode:{:d} R:{:f}"
    STATIC_F = "statistics:[('average_q', {:f}), ('average_loss', {:f}), ('cumulative_steps', {:d}), ('n_updates', {:d}), ('rlen', {:d})]"

    def __init__(self, log_dir):
        self.writer = SummaryWriter(log_dir)

    def info(self, *args):
        if len(args) > 1:
            message = args[0] % args[1:]
        else:
            message = args[0]
        try:
            if "outdir" in message:
                vals = parse.parse(self.STEP_F, message)
                self.writer.add_scalar("base_info/episode", vals[2], vals[1])
                self.writer.add_scalar("base_info/R", vals[3], vals[1])
            elif "statistics" in message:
                vals = parse.parse(self.STATIC_F, message)
                if vals is not None:
                    self.writer.add_scalar("statics/average_q", vals[0], vals[2])
                    self.writer.add_scalar("statics/average_loss", vals[1], vals[2])
                    self.writer.add_scalar("statics/n_updates", vals[3], vals[2])
                    self.writer.add_scalar("statics/rlen", vals[4], vals[2])
            else:
                logging.info(message)
                pass
        except TypeError:
            logging.info(message)


class MyDistributionalDuelingDQN(DistributionalDuelingDQN):
    """Distributional dueling fully-connected Q-function with discrete actions."""

    def __init__(
        self,
        n_actions,
        n_atoms,
        v_min,
        v_max,
        n_input_channels=4,
        activation=torch.relu,
        bias=0.1,
    ):
        super().__init__(n_actions,
                         n_atoms,
                         v_min,
                         v_max,
                         n_input_channels=4,
                         activation=torch.relu,
                         bias=0.1,)
        self.conv_layers = nn.ModuleList(
            [
                nn.Conv2d(n_input_channels, 32, 4, stride=2),
                nn.Conv2d(32, 32, 4, stride=2),
                nn.Conv2d(32, 64, 4, stride=2),
                nn.Conv2d(64, 64, 3, stride=1),
            ]
        )
        # self.main_stream = nn.Linear(64*9*9, 1024)
        # self.main_stream = nn.Linear(64*2*2, 1024)
        self.main_stream = nn.Linear(64*5*5, 1024)


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument(
        "--outdir",
        type=str,
        default="results",
        help=(
            "Directory path to save output files."
            " If it does not exist, it will be created."
        ),
    )
    parser.add_argument("--seed", type=int, default=0,
                        help="Random seed [0, 2 ** 31)")
    parser.add_argument("--gpu", type=int, default=0)
    parser.add_argument("--demo", action="store_true", default=False)
    parser.add_argument("--load-pretrained",
                        action="store_true", default=False)
    parser.add_argument(
        "--pretrained-type", type=str, default="best", choices=["best", "final"]
    )
    parser.add_argument("--load", type=str, default=None)
    parser.add_argument("--eval-epsilon", type=float, default=0.0)
    parser.add_argument("--noisy-net-sigma", type=float, default=0.2)
    parser.add_argument("--steps", type=int, default=5 * 10 ** 7)
    parser.add_argument(
        "--max-frames",
        type=int,
        default=30 * 60 * 60,  # 30 minutes with 60 fps
        help="Maximum number of frames for each episode.",
    )
    parser.add_argument("--replay-start-size", type=int, default=2 * 10 ** 3)
    parser.add_argument("--eval-n-steps", type=int, default=125000)
    parser.add_argument("--eval-interval", type=int, default=250000)
    parser.add_argument(
        "--log-level",
        type=int,
        default=20,
        help="Logging level. 10:DEBUG, 20:INFO etc.",
    )
    parser.add_argument(
        "--render",
        action="store_true",
        default=False,
        help="Render env states in a GUI window.",
    )
    parser.add_argument(
        "--monitor",
        action="store_true",
        default=False,
        help=(
            "Monitor env. Videos and additional information are saved as output files."
        ),
    )
    parser.add_argument("--n-best-episodes", type=int, default=200)
    args = parser.parse_args()

    import logging

    logging.basicConfig(level=args.log_level)

    # Set a random seed used in PFRL.
    utils.set_random_seed(args.seed)

    # Set different random seeds for train and test envs.
    train_seed = args.seed
    test_seed = 2 ** 31 - 1 - args.seed

    test_ID = datetime.datetime.now().strftime("%Y-%m-%d_%H_%M_%S")
    args.outdir = experiments.prepare_output_dir(args, args.outdir, test_ID)
    print("Output files are saved in {}".format(args.outdir))

    env = MapRootEnv()
    eval_env = MapRootEnv()

    n_actions = env.action_space.n
    input_shape = env.input_shape

    n_atoms = 51
    v_max = 10
    v_min = -10
    q_func = MyDistributionalDuelingDQN(
        n_actions,
        n_atoms,
        v_min,
        v_max,
        input_shape[2]
    )

    # Noisy nets
    pnn.to_factorized_noisy(q_func, sigma_scale=args.noisy_net_sigma)
    # Turn off explorer
    explorer = explorers.Greedy()

    # Use the same hyper parameters as https://arxiv.org/abs/1710.02298
    opt = torch.optim.Adam(q_func.parameters(), 6.25e-5, eps=1.5 * 10 ** -4)

    # Prioritized Replay
    # Anneal beta from beta0 to 1 throughout training
    update_interval = 4
    betasteps = args.steps / update_interval
    rbuf = replay_buffers.PrioritizedReplayBuffer(
        5*10 ** 4,
        alpha=0.5,
        beta0=0.4,
        betasteps=betasteps,
        num_steps=3,
        normalize_by_max="batch",
    )

    Agent = agents.CategoricalDoubleDQN
    agent = Agent(
        q_func,
        opt,
        rbuf,
        gpu=args.gpu,
        gamma=0.80,
        explorer=explorer,
        minibatch_size=32,
        replay_start_size=args.replay_start_size,
        target_update_interval=32000,
        update_interval=update_interval,
        batch_accumulator="mean"
    )

    if args.demo:
        eval_stats = experiments.eval_performance(
            env=eval_env, agent=agent, n_steps=args.eval_n_steps, n_episodes=None
        )
        print(
            "n_episodes: {} mean: {} median: {} stdev {}".format(
                eval_stats["episodes"],
                eval_stats["mean"],
                eval_stats["median"],
                eval_stats["stdev"],
            )
        )

    else:
        experiments.train_agent_with_evaluation(
            agent=agent,
            env=env,
            steps=args.steps,
            eval_n_steps=args.eval_n_steps,
            eval_n_episodes=None,
            eval_interval=args.eval_interval,
            outdir=args.outdir,
            save_best_so_far_agent=True,
            eval_env=eval_env,
            logger=TBLogger(args.outdir)
        )

        dir_of_best_network = os.path.join(args.outdir, "best")
        agent.load(dir_of_best_network)

        # run 200 evaluation episodes, each capped at 30 mins of play
        stats = experiments.evaluator.eval_performance(
            env=eval_env,
            agent=agent,
            n_steps=None,
            n_episodes=args.n_best_episodes,
            max_episode_len=args.max_frames / 4,
            logger=None
        )
        with open(os.path.join(args.outdir, "bestscores.json"), "w") as f:
            json.dump(stats, f)
        print("The results of the best scoring network:")
        for stat in stats:
            print(str(stat) + ":" + str(stats[stat]))


if __name__ == "__main__":
    main()

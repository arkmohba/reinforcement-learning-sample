
from tqdm import tqdm
import cv2
import time
from pfrl import nn as pnn
from pfrl import agents, replay_buffers
from map_env_cy import MapRootEnv
from train_rainbow import MyDistributionalDuelingDQN

def main():
    env = MapRootEnv()

    size = env.observation_space.shape[:2]  # 動画の画面サイズ
    size = (size[1], size[0])  # width, heightの順
    action_size = 4
    n_atoms = 51
    v_max = 10
    v_min = -10
    q_func = MyDistributionalDuelingDQN(
        action_size,
        n_atoms,
        v_min,
        v_max,
        env.input_shape[2]
    )
    pnn.to_factorized_noisy(q_func)
    # print(q_func.main_stream.mu)
    agent = agents.CategoricalDoubleDQN(
        q_func,
        None,
        replay_buffers.PrioritizedReplayBuffer(),
        0.99,
        None,
        gpu=0)
    agent.load("results\\first_end\\best")

    final_reward = 0
    obs = env.reset()
    with agent.eval_mode():
        while True:
            env.render()
            action = agent.act(obs)
            obs, reward, done, _ = env.step(action)
            final_reward += reward
            # print("reward:", reward)
            if done:
                print("Complete!")
                break
        print("final reward:", final_reward)
        trail = env.env.get_trail()
        print("trail len: ", len(trail))
        env.render_trailed_image()
        cv2.waitKey(0)


if __name__ == "__main__":
    main()

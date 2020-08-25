import particles
import numpy as np
import gym

from tensorflow.keras.models import Sequential
from tensorflow.keras.layers import Dense, Activation, Flatten, Convolution2D, Dropout, MaxPool2D
from tensorflow.keras.optimizers import Adam
import matplotlib.pyplot as plt

from rl.agents.dqn import DQNAgent
from rl.policy import BoltzmannQPolicy
from rl.memory import SequentialMemory


ENV_NAME = "Bnuts-v0"


# Get the environment and extract the number of actions.
def initilize_env(env_name):
    env = gym.make(env_name)
    np.random.seed(123)
    env.seed(123)
    return env


def create_model(space_shape, nb_actions):
    # Next, we build a very simple model.
    model = Sequential()
    model.add(Convolution2D(32, 3, activation='relu', input_shape=space_shape))
    model.add(MaxPool2D())
    model.add(Convolution2D(32, 3, activation='relu', input_shape=space_shape))
    model.add(MaxPool2D())
    model.add(Convolution2D(32, 3, activation='relu', input_shape=space_shape))
    model.add(MaxPool2D())
    model.add(Convolution2D(32, 3, activation='relu', input_shape=space_shape))
    model.add(MaxPool2D())
    model.add(Convolution2D(16, 3, activation='relu', input_shape=space_shape))
    model.add(Flatten())
    model.add(Dense(256))
    model.add(Activation('relu'))
    model.add(Dropout(0.5))
    model.add(Dense(nb_actions))
    model.add(Activation('linear'))
    print(model.summary())
    return model


def setup_dqn(model, nb_actions):
    # Finally, we configure and compile our agent. You can use every built-in tensorflow.keras optimizer and
    # even the metrics!
    memory = SequentialMemory(limit=50000, window_length=1)
    policy = BoltzmannQPolicy()
    dqn = DQNAgent(model=model, nb_actions=nb_actions, memory=memory, nb_steps_warmup=10,
                   target_model_update=1e-2, policy=policy)
    dqn.compile(Adam(lr=1e-3), metrics=['mae'])
    return dqn


def do_train(dqn: DQNAgent, env, save_path):
    # Okay, now it's time to learn something! We visualize the training here for show, but this
    # slows down training quite a lot. You can always safely abort the training prematurely using
    # Ctrl + C.
    history = dqn.fit(env, nb_steps=200, visualize=False, verbose=0)
    history = history.history

    # After training is done, we save the final weights.
    dqn.save_weights(save_path, overwrite=True)

    # Finally, evaluate our algorithm for 5 episodes.
    dqn.test(env, nb_episodes=5, visualize=True)

    return history

def demo(dqn: DQNAgent, env, model_path):
    dqn.load_weights(model_path)

    dqn.test(env, nb_episodes=5, visualize=True)


def main():
    env = initilize_env(ENV_NAME)
    space_shape = env.observation_space.shape
    print(space_shape)
    nb_actions = env.action_space.n
    print(nb_actions)

    model = create_model(space_shape, nb_actions)

    dqn = setup_dqn(model, nb_actions)

    model_path = "model_{}.h5".format(ENV_NAME)
    history = do_train(dqn, env, model_path)
    # print(history)
    steps = history["nb_steps"]
    hold_step = history["nb_episode_steps"]
    rewards = history["episode_reward"]
    plt.plot(rewards, label="reward")
    # plt.plot(hold_step, label="hold_steps")
    plt.legend()
    plt.show()
    # demo(dqn, env, model_path)


if __name__ == "__main__":
    main()

import particles
import gym
import time
from tqdm import tqdm


def main():
    env_name = "Bnuts-v0"
    env = gym.make(env_name)
    for _ in tqdm(range(10000)):
        env.render()
        env.step(1)
        # time.sleep(0.001)


if __name__ == "__main__":
    main()

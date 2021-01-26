
from tqdm import tqdm
import cv2
from map_env_cy import MapRootEnv
import time
import numpy as np


def main():
    env = MapRootEnv()

    size = env.observation_space.shape[:2]  # 動画の画面サイズ
    size = (size[1], size[0])  # width, heightの順

    action_size = 4

    start = time.time()
    n_actions = 0
    n_repeat = 10
    for _ in range(10):
        env.reset()
        while 500:
            action = np.random.randint(0, action_size-1)
            obs, reward, done, _ = env.step(action)
            n_actions += 1
            if done:
                print("Complete!")
                break
    end = time.time()
    print(f"average time {(end - start)/n_actions} sec/action")


if __name__ == "__main__":
    main()

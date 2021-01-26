
from tqdm import tqdm
import cv2
from map_env_cy import MapRootEnv
import time


def main():
    env = MapRootEnv()

    size = env.observation_space.shape[:2]  # 動画の画面サイズ
    size = (size[1], size[0])  # width, heightの順

    final_reward = 0
    while True:
        env.render()
        k = cv2.waitKey(0) & 0xFF
        action = 0
        if k == ord('u'):
            action = 0
        elif k == ord('r'):
            action = 1
        elif k == ord('d'):
            action = 2
        elif k == ord('l'):
            action = 3
        elif k == ord('q'):
            break
        else:
            continue
        obs, reward, done, _ = env.step(action)
        final_reward += reward
        print("reward:", reward)
        if done:
            print("Complete!")
            break
    print("final reward:", final_reward)
    env.render_trailed_image()
    cv2.waitKey(0)


if __name__ == "__main__":
    main()

import particles
import gym
import time
from tqdm import tqdm
import random
import math
import cv2
from particle_rl import create_model, setup_dqn


def main():
    env_name = "Bnuts-v1"
    env = gym.make(env_name)

    frame_rate = 30  # フレームレート
    size = env.observation_space.shape[:2]  # 動画の画面サイズ
    size = (size[1], size[0])  # width, heightの順

    fmt = cv2.VideoWriter_fourcc('m', 'p', '4', 'v')  # ファイル形式(ここではmp4)
    writer = cv2.VideoWriter('./output_demo.mp4', fmt,
                             frame_rate, size)  # ライター作成

    model = create_model(env.observation_space.shape, env.action_space.n)
    dqn = setup_dqn(model, env.action_space.n)
    model_name = "model_{}.h5".format(env_name)
    dqn.load_weights(model_name)
    obs, _, _, _ = env.step(0)
    for _ in tqdm(range(1000)):
        env.render()
        action = dqn.forward(obs)
        obs, _, _, _ = env.step(action)
        writer.write(obs)  # 画像を1フレーム分として書き込み
        # time.sleep(0.001)

    writer.release()  # ファイルを閉じるs


if __name__ == "__main__":
    main()

import particles
import sys

import gym
import numpy as np
import gym.spaces

from gym.envs.classic_control import rendering
from . import BNutsEnvCore


class BNutsEnv(gym.Env):
    metadata = {'render.modes': ['human', 'ansi']}
    MAX_STEPS = 10000

    def __init__(self):
        self.steps = 0
        self.viewer = rendering.SimpleImageViewer()

        self.env_body: BNutsEnv = BNutsEnvCore()

        # 実際の環境からshapeを取得する
        width = self.env_body.get_width()
        height = self.env_body.get_height()
        channel = self.env_body.get_channels()
        self.input_shape = [height, width, channel]

        # 実際の環境からmax_diffを取得する
        # TODO 場合によっては直接高さを指定するのではなく差分だけを指定する方式に変更する
        self.max_diff = self.env_body.get_max_diff()
        self.separate = 10  # 更に10倍に分割

        # 行動の個数
        self.action_space = gym.spaces.Discrete(
            int(self.max_diff*self.separate))

        # 画像データの情報（ピクセルの最大、最小）
        self.observation_space = gym.spaces.Box(
            low=0,
            high=255,
            shape=self.input_shape
        )

        # 報酬の最大最小
        self.reward_range = [-height, height]
        self.reset()
        pass

    def step(self, action):
        """actionは数字"""
        # 行動値
        # TODO 場合によっては直接高さを指定するのではなく差分だけを指定する方式に変更する
        base = action / self.separate

        # 更新
        # 行動値を入力する
        self.env_body.update_with_base(base)

        # 画像データの取得
        observation = self.env_body.get_image_array()

        # 報酬の取得（ブラジルナッツの位置と他の最高値を取得して差分を計算する）
        bnats_y = self.env_body.get_bnuts_y()
        other_min_y = self.env_body.get_others_min_y()
        reward = other_min_y - bnats_y

        # 終了したかどうか
        self.done = self._is_done(bnats_y, other_min_y)
        return observation, reward, self.done, {}

    def reset(self):
        self.steps = 0
        self.done = False
        self.env_body.init()
        return self.env_body.get_image_array()

    def render(self, mode='human', close=False):
        img = self.env_body.get_image_array()
        self.viewer.imshow(img[:, :, ::-1])

    def _is_done(self, bnats_y, other_max_y):
        if other_max_y > bnats_y:
            return True
        elif self.steps > self.MAX_STEPS:
            return True

    def _close(self):
        pass

    def _seed(self, seed=None):
        pass

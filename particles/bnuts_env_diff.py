import particles
import sys

import gym
import numpy as np
import gym.spaces

from gym.envs.classic_control import rendering
from .bnuts_env import BNutsEnv


class BNutsEnvDiff(BNutsEnv):
    """床の高さのズレを指定する形式のシミュレーション"""

    def __init__(self):
        super().__init__()
        self.now_state = 0
        # 行動の個数
        self.action_space = gym.spaces.Discrete(
            int(self.separate))

    def get_base(self, action):
        self.now_state += (action - self.separate/2) * 2  # [-1, 1]
        self.now_state = min(max(self.now_state, 0), self.max_diff)
        return self.now_state

    def reset(self):
        self.now_state = 0
        return super().reset()

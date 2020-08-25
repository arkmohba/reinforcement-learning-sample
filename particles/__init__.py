from gym.envs.registration import register
from .bnuts.build.Debug.bnutsenv import BNutsEnvCore

register(
    id='Bnuts-v0',
    entry_point='particles.bnuts_env:BNutsEnv'
)
print("registered")
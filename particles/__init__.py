from gym.envs.registration import register

register(
    id='Nnuts-v0',
    entry_point='particles.bnuts_env:BNutsEnv'
)
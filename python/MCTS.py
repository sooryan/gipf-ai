import time
import numpy as np
from operator import itemgetter


def rollout_policy_fn(state):
    legal_moves = state.get_legal_moves()
    action_probs = np.random.rand(len(legal_moves))
    return zip(legal_moves, action_probs)


def policy_value_fn(state):
    legal_moves = state.get_legal_moves()
    action_probs = np.ones(len(legal_moves)) / len(legal_moves)
    return zip(legal_moves, action_probs), 0


class TreeNode(object):
    def __init__(self, parent, prior_p):
        self._parent = parent
        self._children = {}
        self._n_visits = 0
        self._Q = 0
        self._u = 0
        self._P = prior_p

    def expand(self, action_priors):
        for action, prob in action_priors:
            if action not in self._children:
                self._children[action] = TreeNode(self, prob)

    def select(self, c_puct):
        return max(self._children.items(),
                   key=lambda act_node: act_node[1].get_value(c_puct))

    def update(self, leaf_value):
        self._n_visits += 1
        self._Q += 1.0 * (leaf_value - self._Q) / self._n_visits

    def update_recursive(self, leaf_value):
        if self._parent:
            self._parent.update_recursive(-leaf_value)
        self.update(leaf_value)

    def get_value(self, c_puct):
        self._u = (c_puct * self._P *
                   np.sqrt(self._parent._n_visits) / (1 + self._n_visits))
        return self._Q + self._u

    def is_leaf(self):
        return self._children == {}

    def is_root(self):
        return self._parent is None


class MCTS(object):
    def __init__(self, policy_value_fn, c_puct=5, t_playout=3):
        self._root = TreeNode(None, 1.0)
        self._policy = policy_value_fn
        self._c_puct = c_puct
        self._t_playout = t_playout

    def _playout(self, state):
        node = self._root
        while(1):
            if node.is_leaf():
                break
            action, node = node.select(self._c_puct)
            state.make_move(action)
        action_probs, _ = self._policy(state)
        end = state.is_terminal()
        if not end:
            node.expand(action_probs)
        leaf_value = self._evaluate_rollout(state)
        node.update_recursive(-leaf_value)

    def _evaluate_rollout(self, state, limit=1000):
        for i in range(limit):
            end = state.is_terminal()
            if end:
                break
            action_probs = rollout_policy_fn(state)
            max_action = max(action_probs, key=itemgetter(1))[0]
            state.make_move(max_action)
        else:
            print("WARNING: rollout reached move limit")
        if state.is_winner(state.player_to_move):
            return 1
        elif state.is_winner(state.get_enemy(state.player_to_move)):
            return -1
        else:
            return 0

    def get_move(self, state):
        start_time = time.time()
        while time.time() < start_time + self._t_playout:
            state_copy = state.clone()
            self._playout(state_copy)
        return max(self._root._children.items(),
                   key=lambda act_node: act_node[1]._n_visits)[0]

    def update_with_move(self, last_move):
        if last_move in self._root._children:
            self._root = self._root._children[last_move]
            self._root._parent = None
        else:
            self._root = TreeNode(None, 1.0)

    def __str__(self):
        return "MCTS"


class MCTSPlayer(object):
    def __init__(self, c_puct=5, t_playout=3):
        self.mcts = MCTS(policy_value_fn, c_puct, t_playout)

    def set_player_ind(self, p):
        self.player = p

    def reset_player(self):
        self.mcts.update_with_move(-1)

    def get_action(self, state):
        sensible_moves = state.get_legal_moves()
        if len(sensible_moves) > 0:
            move = self.mcts.get_move(state)
            self.mcts.update_with_move(-1)
            return move
        else:
            print("WARNING: the board is full")

    def __str__(self):
        return "MCTS {}".format(self.player)

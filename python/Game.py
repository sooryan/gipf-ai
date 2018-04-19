import gipf
import os
from MCTS import MCTSPlayer


class Game(object):
    def __init__(self, state, **kwargs):
        self.state = state

    def graphic(self, state):
        print(state)

    def start_play(self, player1, player2, is_shown=1):
        p1, p2 = ['1', '2']
        player1.set_player_ind(p1)
        player2.set_player_ind(p2)
        players = {p1: player1, p2: player2}
        if is_shown:
            os.system("clear")
            self.graphic(self.state)
        while True:
            current_player = self.state.player_to_move
            player_in_turn = players[current_player]
            move = player_in_turn.get_action(self.state)
            self.state.make_move(move)
            if is_shown:
                os.system("clear")
                self.graphic(self.state)
            end = self.state.is_terminal()
            if end:
                winner = ''
                if self.state.is_winner(p1):
                    winner = p1
                elif self.state.is_winner(p2):
                    winner = p2
                if is_shown:
                    if winner != '':
                        print("Game end. Winner is", players[winner])
                    else:
                        print("Game end. Tie")
                return winner


def main():
    state = gipf.GipfState()
    game = Game(state)
    player1 = MCTSPlayer(t_playout=3)
    player2 = MCTSPlayer(t_playout=3)
    game.start_play(player1, player2)


if __name__ == '__main__':
    main()

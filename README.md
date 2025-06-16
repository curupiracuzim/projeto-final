# Torre de Hanói - Jogo em C

Este projeto é uma implementação do clássico jogo **Torre de Hanói** em linguagem C, jogado via terminal. O jogo permite ao usuário escolher o número de discos, realizar movimentos entre as torres e salva o histórico das partidas em um arquivo.

## Como jogar



2. **Siga as instruções do terminal:**
   - Informe seu nome.
   - Escolha a quantidade de discos (entre 3 e 8).
   - Para cada movimento, escolha a torre de origem e a torre de destino usando os números:
     - 0 = Torre A
     - 1 = Torre B
     - 2 = Torre C
   - Digite `-1` como torre de origem para abandonar o jogo.

3. **Histórico:**
   - Ao final da partida, você pode visualizar o histórico de jogos realizados.
   - O histórico é salvo no arquivo `historico_hanoi.txt`.

## Regras do Jogo

- Apenas um disco pode ser movido por vez.
- Um disco maior nunca pode ficar sobre um disco menor.
- O objetivo é mover todos os discos da Torre A para a Torre C, usando a Torre B como auxiliar.

## Estrutura do Projeto

- `main.c`: Código-fonte principal do jogo.
- `historico_hanoi.txt`: Arquivo gerado automaticamente para armazenar o histórico das partidas.

## Créditos

Desenvolvido por gabriel alberto duarte de andrade.

---
Divirta-se jogando Torre de Hanói professor robson!
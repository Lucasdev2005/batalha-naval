# 🛥️ Batalha Naval (em C)

Um jogo de **Batalha Naval** em terminal, feito em linguagem C. Enfrente a CPU e tente afundar todos os navios inimigos antes que ela destrua os seus!

---

## 🚀 Como compilar e executar

### 🧩 Pré-requisitos

* GCC (ou outro compilador C)
* Terminal (Linux, macOS ou Windows com WSL / MinGW)

### ⚙️ Compilação

```bash
gcc -o batalhanaval BatalhaNaval.c
```

### ▶️ Execução

```bash
./batalhanaval
```

---

## 🎮 Como jogar

1. Ao iniciar, o jogo perguntará se você deseja **posicionar seus navios manualmente** (`s`) ou **automaticamente** (`n`).
2. Se escolher manual, use o formato:

   ```
   A1 H
   ```

   Onde:

   * `A1` é a posição inicial (linha A, coluna 1)
   * `H` indica orientação **horizontal** (use `V` para vertical)
3. Durante a partida, o jogador e a CPU se alternam para atirar:

   ```
   A5
   ```

   * `A5` = Linha A, Coluna 5
4. O jogo termina quando **todos os navios de um dos lados** forem afundados.

Digite `sair` para encerrar a partida a qualquer momento.

---

## ⚓ Embarcações

| Tipo         | Tamanho |
| ------------ | ------- |
| Porta-aviões | 5       |
| Encouraçado  | 4       |
| Cruzador     | 3       |
| Submarino    | 3       |
| Destroyer    | 2       |

---

## 🧭 Símbolos do tabuleiro

| Símbolo | Significado            |
| ------- | ---------------------- |
| `~`     | Água                   |
| `S`     | Navio (somente no seu) |
| `X`     | Acerto                 |
| `o`     | Tiro na água (erro)    |

---

## 🤖 Inteligência Artificial

A CPU realiza ataques **aleatórios** em posições que ainda não foram atingidas. (Futuramente, pode ser melhorada para perseguir navios acertados.)

---

## 🧠 Estrutura do código

O código está organizado de forma simples, em um único arquivo:

* `init_board` – inicializa o tabuleiro
* `place_ships_random` / `player_place_manual` – posicionamento dos navios
* `attack` – lógica de disparos
* `print_boards` – exibição dos tabuleiros
* `cpu_choose_shot` – escolha do tiro do inimigo

---

## 📜 Licença

Este projeto é de código aberto e pode ser usado livremente para fins educacionais e pessoais.

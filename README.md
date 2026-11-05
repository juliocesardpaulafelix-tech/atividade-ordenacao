# Competição de Ordenação — O Desafio dos Milissegundos

**Estratégia:** Counting Sort, O(N + K). A base tem 10.000.000 inteiros no intervalo [1, 2.500.000]
(K << N), então contar ocorrências é muito mais rápido que qualquer ordenação por comparação.
Se o intervalo for muito grande, o programa usa automaticamente um LSD Radix Sort (também O(N)).

## Compilar
    gcc -O2 -o ordenacao ordenacao.c

## Executar
    ./ordenacao listaOdenada.txt

## Medição
`clock()` envolve **apenas** a função `ordenar()`. Leitura do arquivo e verificação ficam fora.
A verificação confere ordem crescente e preservação dos elementos (soma + hash).

## Resultado (print do terminal)
Cole aqui o print da execução.

## Equipe
1. João Francisco Cabral de Oliveira — Líder
2. Igor Dayan da Silva Martins.
3. Pedro Oliveira Daleprani.
4. Julio César de Paula Felix.
5. Nathan Jonathan Amaral Moreira.

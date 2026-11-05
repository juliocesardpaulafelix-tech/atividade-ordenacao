/*
 * Competição de Ordenação: O Desafio dos Milissegundos
 *
 * Estratégia: Counting Sort (ordenação por contagem), O(N + K), onde K = max - min + 1.
 *  - O arquivo tem 10.000.000 inteiros no intervalo [1, 2.500.000] com muitas repetições,
 *    ou seja, K << N: cenário ideal para ordenação por contagem (sem comparações).
 *  - Se o intervalo for grande demais (K > LIMITE_K), cai para um LSD Radix Sort
 *    (4 passadas de 8 bits), que também é O(N) e funciona para qualquer inteiro de 32 bits.
 *
 * Medição: clock() envolve SOMENTE a chamada de ordenar(). Leitura do arquivo e
 * verificação de corretude ficam fora do cronômetro.
 *
 * Compilar:  gcc -O2 -o ordenacao ordenacao.c
 * Executar:  ./ordenacao listaOdenada.txt
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <time.h>

#define LIMITE_K (64u * 1024u * 1024u)  /* máx. de contadores no counting sort (256 MB) */

/* ---------- Leitura (fora da medição): lê o arquivo inteiro e converte para int ---------- */
static int *ler_arquivo(const char *caminho, size_t *n_out) {
    FILE *f = fopen(caminho, "rb");
    if (!f) { perror("fopen"); exit(1); }
    fseek(f, 0, SEEK_END);
    long tam = ftell(f);
    rewind(f);

    char *buf = malloc((size_t)tam + 1);
    if (!buf || fread(buf, 1, (size_t)tam, f) != (size_t)tam) { fprintf(stderr, "erro de leitura\n"); exit(1); }
    fclose(f);
    buf[tam] = '\0';

    size_t cap = (size_t)tam / 2 + 16, n = 0;   /* cada número ocupa >= 2 bytes (dígito + \n) */
    int *v = malloc(cap * sizeof(int));
    if (!v) { fprintf(stderr, "sem memória\n"); exit(1); }

    char *p = buf, *fim = buf + tam;
    while (p < fim) {
        while (p < fim && (*p < '0' || *p > '9') && *p != '-') p++;
        if (p >= fim) break;
        int neg = (*p == '-'); if (neg) p++;
        long x = 0;
        while (p < fim && *p >= '0' && *p <= '9') x = x * 10 + (*p++ - '0');
        v[n++] = (int)(neg ? -x : x);
    }
    free(buf);
    *n_out = n;
    return v;
}

/* ---------- Counting Sort ---------- */
static int counting_sort(int *v, size_t n, int mn, int mx) {
    size_t k = (size_t)((int64_t)mx - mn + 1);
    uint32_t *cont = calloc(k, sizeof(uint32_t));
    if (!cont) return 0;
    for (size_t i = 0; i < n; i++) cont[v[i] - mn]++;
    size_t pos = 0;
    for (size_t j = 0; j < k; j++) {
        uint32_t c = cont[j];
        int val = (int)(j + mn);
        while (c--) v[pos++] = val;
    }
    free(cont);
    return 1;
}

/* ---------- LSD Radix Sort (fallback para intervalos grandes) ---------- */
static void radix_sort(int *v, size_t n) {
    uint32_t *a = (uint32_t *)v, *b = malloc(n * sizeof(uint32_t));
    if (!b) { fprintf(stderr, "sem memória\n"); exit(1); }
    for (size_t i = 0; i < n; i++) a[i] ^= 0x80000000u;      /* suporta negativos */
    for (int s = 0; s < 32; s += 8) {
        size_t c[257] = {0};
        for (size_t i = 0; i < n; i++) c[((a[i] >> s) & 0xFF) + 1]++;
        for (int i = 0; i < 256; i++) c[i + 1] += c[i];
        for (size_t i = 0; i < n; i++) b[c[(a[i] >> s) & 0xFF]++] = a[i];
        uint32_t *t = a; a = b; b = t;
    }                                                         /* 4 passadas: resultado volta em v */
    for (size_t i = 0; i < n; i++) ((uint32_t *)v)[i] ^= 0x80000000u;
    free(b);
}

static void ordenar(int *v, size_t n) {
    if (n < 2) return;
    int mn = v[0], mx = v[0];
    for (size_t i = 1; i < n; i++) { if (v[i] < mn) mn = v[i]; if (v[i] > mx) mx = v[i]; }
    if ((int64_t)mx - mn + 1 <= (int64_t)LIMITE_K && counting_sort(v, n, mn, mx)) return;
    radix_sort(v, n);
}

/* ---------- Verificação (fora da medição) ---------- */
static int verificar(const int *v, size_t n, uint64_t soma_ant, uint64_t xor_ant) {
    uint64_t soma = 0, x = 0;
    for (size_t i = 0; i < n; i++) {
        if (i && v[i - 1] > v[i]) return 0;      /* fora de ordem */
        soma += (uint64_t)(uint32_t)v[i]; x ^= (uint64_t)(uint32_t)v[i] * 0x9E3779B97F4A7C15ull;
    }
    return soma == soma_ant && x == xor_ant;     /* nenhum elemento perdido/alterado */
}

int main(int argc, char **argv) {
    const char *arq = argc > 1 ? argv[1] : "ListaASerOrdenada.txt";
    size_t n;
    int *v = ler_arquivo(arq, &n);               /* leitura NÃO é cronometrada */
    printf("Elementos lidos: %zu\n", n);

    uint64_t soma = 0, x = 0;
    for (size_t i = 0; i < n; i++) { soma += (uint64_t)(uint32_t)v[i]; x ^= (uint64_t)(uint32_t)v[i] * 0x9E3779B97F4A7C15ull; }

    clock_t inicio = clock();
    ordenar(v, n);
    clock_t fim = clock();

    double seg = (double)(fim - inicio) / CLOCKS_PER_SEC;
    printf("Tempo de ordenacao: %f segundos (%f ms)\n", seg, seg * 1000.0);
    printf("Verificacao: %s\n", verificar(v, n, soma, x) ? "OK (ordem crescente, sem perda de elementos)" : "FALHOU");
    free(v);
    return 0;
}

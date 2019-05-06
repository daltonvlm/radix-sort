/*
 * Trabalho de Estruturas de Dados 
 *
 * Implementar um programa que recebe como entrada inteiros n e d e gera
 * aleatoriamente uma lista com n elementos com exatamente d dígitos (o 
 * dígito mais significativo pode ser 0). O programa deve listar os n 
 * elementos gerados e, a seguir, quando solicitado, executar a ordenação 
 * por distribuição. O programa deve imprimir, a cada iteração, o estado 
 * corrente de b filas, bem como o estado corrente da lista de elementos a 
 * ser ordenada.
 *
 * Considere b = 10.
 * As impressões do programa devem seguir o modelo do livro-texto (pág. 
 * 35).
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <libgen.h>
#include <time.h>

#define MAXVAL 	10
#define BASE 	10

/* Nós da fila a serem alocados dinamicamente */
typedef struct numero {
	char *numero;
	struct numero *prox;
} Numero;

char **gera_numeros(size_t n, size_t d);
void libera_numeros(char **numeros, size_t n);
void inicializa_fila(Numero *fila[]);
void libera_fila(Numero *fila[]);
void mostra_numeros(char **numeros, size_t n);
void radix_sort(Numero *fila[], char **numeros, size_t n, size_t d);

void parseargs(int argc, char **argv, size_t *n, size_t *d);

int main(int argc, char **argv)
{
	size_t n, d;
	char **numeros;			/* ponteiro para sequência de números em 
							   formato de string */
	Numero *fila[BASE];

	/* Define valor aleatório para rand() */
	srand(time(NULL));

	/* +1 para garantir que nem n, nem d terão 0 como valor */
	n = rand() % MAXVAL + 1;
	d = rand() % MAXVAL + 1;
	parseargs(argc, argv, &n, &d);
	numeros = gera_numeros(n, d);

	/* Define como NULL o valor inicial de cada elemento de 'fila' */
	inicializa_fila(fila);

	mostra_numeros(numeros, n);
		
	/* Ordena os valores em 'numeros' */
	radix_sort(fila, numeros, n, d);

	/* Libera elementos alocados dinamicamente */
	libera_numeros(numeros, n);
	libera_fila(fila);

	return EXIT_SUCCESS;
}

static void *aloca(size_t n)
{
	void *p = malloc(n);

	if (!p) {
		perror("aloca");
		exit(EXIT_FAILURE);
	}
	return p;
}

static char *gera_numero(size_t d)
{
	char *p, *numero;

	p = numero = (char *) aloca((d + 1) * sizeof(char));
	while (d--)
		*p++ = '0' + rand() % BASE;
	*p = '\0';
	return numero;
}

char **gera_numeros(size_t n, size_t d)
{
	char **p, **numeros;

	p = numeros = (char **) aloca(n * sizeof(char *));
	while (n--)
		*p++ = gera_numero(d);
	return numeros;
}

void libera_numeros(char **numeros, size_t n)
{
	while (n--)
		free(*numeros++);
}

void libera_fila(Numero *fila[])
{
	int i;
	Numero *p, *t;

	/* Libera apenas os nós, não as informações */
	for (i = 0; i < BASE; i++) {
		p = fila[i];
		while (p) {
			t = p;
			p = p->prox;
			free(t);
		}
		fila[i] = NULL;
	}
}

void inicializa_fila(Numero *fila[])
{
	int i;

	for (i = 0; i < BASE; i++)
		fila[i] = NULL;
}

static int pega_indice(char *number, size_t d)
{
	return number[d] - '0';
}

static void mostra_fila(Numero *fila[])
{
	int i;
	Numero *p;

	for (i = 0; i < BASE; i++) {
		printf("fila%d:\t", i);
		for (p = fila[i]; p; p = p->prox)
			printf(" %s%c", p->numero, p->prox ? ',' : '\0');
		puts("");
	}
	puts("");
}

void mostra_numeros(char **numeros, size_t n)
{
	int i;

	printf("Tabela:");
	for (i = 0; i < n; i++)
		printf("  %s", numeros[i]);
	puts("");
}

static Numero *cria_numero(char *numero)
{
	Numero *p = (Numero *) aloca(sizeof(Numero));
	p->numero = numero;
	p->prox = NULL;
	return p;
}

void radix_sort(Numero *fila[], char **numeros, size_t n, size_t d)
{
	int pos, i, j;
	static int it = 0;
	Numero **p;

	if (d--) {
		it++;
		printf("\nIteracao %d: %da distribuicao\n\n", it, it);
		for (i = 0; i < n; i++) {
			pos = pega_indice(numeros[i], d);
			for (p = &fila[pos]; *p; p = &(*p)->prox)
				;
			*p = cria_numero(numeros[i]);
		}
		pos = 0;
		for (i = 0; i < BASE; i++)
			for (p = &fila[i]; *p; p = &(*p)->prox)
				numeros[pos++] = (*p)->numero;
		mostra_fila(fila);
		mostra_numeros(numeros, n);
		libera_fila(fila);
		radix_sort(fila, numeros, n, d);
	}
}

static void usage(char *err, ...);

char *progname;

void parseargs(int argc, char **argv, size_t *n, size_t *d)
{

	progname = basename(*argv);
	while (--argc && '-' == **++argv)
		switch (*++*argv) {
			case 'n':
				*n = atoi(*argv + 1);
				break;
			case 'd':
				*d = atoi(*argv + 1);
				break;
			default:
				usage("parseargs: argumento invalido '%c'\n", **argv);
				break;
		}
	if (argc)
		usage("");
}

static void usage(char *err, ...)
{
	va_list va;

	if (*err) {
		va_start(va, err);
		vfprintf(stderr, err, va);
		va_end(va);
	}

	fprintf(stderr,

			"Uso: %s [-n<valor>] [-d<valor>]\n\n"

			"	-n	Quantidade de numeros\n"
			"	-d	Quantidade de digitos\n\n"

			"Se nao especificados, n e d variam entre 1 e %d.\n", 
			progname, MAXVAL
	);
	exit(EXIT_FAILURE);
}

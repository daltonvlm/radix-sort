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
#include <time.h>
#include <stdarg.h>
#include <libgen.h>
#include <ctype.h>

#define STD_N 	5
#define STD_D 	10
#define STD_B 	10
#define MAX_B	(10 + 'z' - 'a' + 1)

typedef struct numero {
	char *numero;
	struct numero *prox;
} Numero;

char *progname;

char **gera_numeros(size_t n, size_t d, size_t b);
void libera_numeros(size_t n, char **numeros);
Numero **gera_fila(size_t b);
void libera_fila(size_t b, Numero **fila);
void radix(size_t b, Numero **fila, size_t n, size_t d, char **numeros);
void parse_args(int argc, char **argv, size_t *n, size_t *d, size_t *b);
void print_numeros(size_t n, char **numeros);

int main(int argc, char **argv)
{
	size_t n, d, b;
	char **numeros;
	Numero **fila;

	srand(time(NULL));
	progname = basename(*argv);
	parse_args(argc, argv, &n, &d, &b);
	numeros = gera_numeros(n, d, b);
	fila = gera_fila(b);
	print_numeros(n, numeros);
	radix(b, fila, n, d, numeros);
	libera_numeros(n, numeros);
	libera_fila(b, fila);

	return EXIT_SUCCESS;
}

static void *aloca(size_t n)
{
	void *p;

	if (!(p = malloc(n))) {
		perror("aloca");
		exit(EXIT_FAILURE);
	}
	return p;
}

static char gera_digito(size_t b)
{
	static char *digitos = "0123456789abcdefghijklmnopqrstuvwxyz";

	return digitos[rand() % b];
}

static char *gera_numero(size_t d, size_t b)
{
	char *p, *numero;

	p = numero = (char *) aloca((d + 1) * sizeof(char));
	while (d--)
		*p++ = gera_digito(b);
	*p = '\0';
	return numero;
}

char **gera_numeros(size_t n, size_t d, size_t b)
{
	char **p, **numeros;

	p = numeros = (char **) aloca(n * sizeof(char *));
	while (n--)
		*p++ = gera_numero(d, b);
	return numeros;
}

void libera_numeros(size_t n, char **numeros)
{
	char **p;

	p = numeros;
	while (n--)
		free(*p++);
	free(numeros);
}

Numero **gera_fila(size_t b)
{
	Numero **p, **fila;

	p = fila = (Numero **) aloca(b * sizeof(Numero *));
	while (b--)
		*p++ = NULL;
	return fila;
}

static void zera_fila(size_t b, Numero **fila)
{
	Numero *p, *tmp;

	for (p = *fila; b--; fila++) {
		while (p) {
			tmp = p;
			p = p->prox;
			free(tmp);
		}
		*fila = NULL;
	}
}

void libera_fila(size_t b, Numero **fila)
{
	zera_fila(b, fila);
	free(fila);
}

static size_t pega_indice(char c)
{
	if (isalpha(c))
		return 10 + c - 'a';
	return c - '0';
}

static void insere_fila(Numero **fila, size_t d, char *numero)
{
	Numero **p;
	size_t i;

	i = pega_indice(numero[d]);
	p = &fila[i];
	while (*p)
		p = &(*p)->prox;
	*p = (Numero *) aloca(sizeof(Numero));
	(*p)->numero = numero;
	(*p)->prox = NULL;
}

void print_numeros(size_t n, char **numeros)
{
	printf("Tabela:");
	while (n--)
		printf("  %s", *numeros++);
	puts("");
}

static void print_fila(size_t b, Numero **fila)
{
	int i;
	Numero *p;

	for (i = 0; i < b; i++) {
		printf("fila%d: ", i);
		for (p = fila[i]; p; p = p->prox)
			printf(" %s%c", p->numero, p->prox ? ',' : '\0');
		puts("");
	}
	puts("");
}

void radix(size_t b, Numero **fila, size_t n, size_t d,
		char **numeros)
{
	int i, j;
	static it = 0;
	Numero *p;

	if (d--) {
		it++;
		printf("\nIteracao %d: %da distribuicao\n\n", it, it);

		for (i = 0; i < n; i++)
			insere_fila(fila, d, numeros[i]);
		i = 0;
		for (j = 0; j < b; j++)
			for (p = fila[j]; p; p = p->prox)
				numeros[i++] = p->numero;

		print_fila(b, fila);
		print_numeros(n, numeros);
		zera_fila(b, fila);
		radix(b, fila, n, d, numeros);
	}
}

static void usage(char *msg, ...)
{
	va_list ap;
	
	if (msg) {
		va_start(ap, msg);
		vfprintf(stderr, msg, ap);
		va_end(ap);
	}
	printf(
		"\n"
		"Uso: %s [OPCAO<VALOR>]\n\n"
		""
		""
		"OPCOES:\n\n"
		""
		""
		"	-n	Quantidade de numeros a serem ordenados\n"
		"	-d	Quantidade de digitos em cada numero\n"
		"	-b	Base de representacao dos numeros\n\n"
		""
		""
		"Os valores padroes para cada opcao sao:\n\n"
		""
		""
		"	-n	%d\n"
		"	-d	%d\n"
		"	-b	%d\n\n",
		progname, STD_N, STD_D, STD_B);

	exit(EXIT_FAILURE);
}

void parse_args(int argc, char **argv, size_t *n, size_t *d, size_t *b)
{
	int tmp;

	/* n e d minimos devem ser 1 */
	*n = 1 + rand() % STD_N;
	*d = 1 + rand() % STD_D;
	/* base minima deve ser 2, e maxima, 36 */
	*b = 2 + rand() % (STD_B - 1);

	while (--argc && '-' == **++argv)
		switch (*++*argv) {
			case 'n':
				tmp = atoi(*argv + 1);
				if (!tmp)
					usage("parseargs: n e d minimos devem ser 1\n");
				*n = tmp;
				break;
			case 'd':
				tmp = atoi(*argv + 1);
				if (!tmp)
					usage("parseargs: n e d minimos devem ser 1\n");
				*d = tmp;
				break;
			case 'b':
				tmp = atoi(*argv + 1);
				if (tmp < 2 || tmp > MAX_B)
					usage("parseargs: base precisa estar em [2, %d]\n",
							MAX_B);
				*b = tmp;
				break;
			default:
				usage("parseargs: argumento invalido '%c'\n",
						**argv);
				break;
		}
	if (argc)
		usage(NULL);
}

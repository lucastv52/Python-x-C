import time
import random
import string

TAM_CHAVE = 10
MAX_N = 1_000_000
PASSO = 50_000

def gerar_chave_aleatoria(tamanho):
    return ''.join(random.choices(string.ascii_letters + string.digits, k=tamanho))

for N in range(0, MAX_N + 1, PASSO):
    if N == 0:
        continue

    print(f"\n------ TESTE COM {N} ELEMENTOS ------")

    chaves = [gerar_chave_aleatoria(TAM_CHAVE) for _ in range(N)]
    valores = [random.randint(0, 100000) for _ in range(N)]
    dicionario = {}

    # Inserção
    inicio = time.perf_counter()
    for chave, valor in zip(chaves, valores):
        dicionario[chave] = valor
    fim = time.perf_counter()
    print(f"Tempo de inserção: {fim - inicio:.6f} s")


    inicio = time.perf_counter()
    for i in range(int(0.9 * N)):
        _ = dicionario.get(chaves[i])
    for _ in range(int(0.1 * N)):
        fake = gerar_chave_aleatoria(TAM_CHAVE)
        _ = dicionario.get(fake)
    fim = time.perf_counter()
    print(f"Tempo de busca: {fim - inicio:.6f} s")

    # Remoção 
    inicio = time.perf_counter()
    for chave in reversed(chaves):
        del dicionario[chave]
    fim = time.perf_counter()
    print(f"Tempo de remoção: {fim - inicio:.6f} s")

# EVOLUTION V2 — 2026-08-27

Estado inicial: `V1_IMPLEMENTED / MEASURED_LOCAL_X86_64 / CROSS_HOST_BOUNDED_PASS`

Estado atual desta evolução: `V2_GRAMMAR_HARDENED / X86_CROSS_HOST_PASS / ARMV7_CROSS_BUILD_VERIFIED / AARCH64_CROSS_BUILD_VERIFIED / claim_allowed=false`

## F_ok herdado

- kernel freestanding V1 executado em x86_64;
- corpus histórico de 792.693.581 bytes medido;
- 2.573 objetos de topo confirmados também por `jq --stream`;
- CI independente V1 concluído com sucesso;
- receipts V1 e manifesto histórico preservados.

## Delta V2

1. adiciona `src/raf_json_syntax.h`, validador streaming fail-closed da gramática JSON;
2. mantém o scanner de contagens V1 como camada independente;
3. saída nova usa schema `rafcodephi.freestanding.stream-json-receipt.v2`;
4. adiciona `json_syntax_errors` e `json_syntax_ok`;
5. expande fixtures manuais de 4 para 18 casos positivos/negativos;
6. adiciona testes diferenciais determinísticos contra `json.loads`, incluindo fronteiras de 64 KiB;
7. adiciona schema V2 com propriedades fechadas e invariantes de sucesso constantes;
8. separa corretamente o schema da saída crua do schema do envelope histórico enriquecido;
9. adiciona validador de contrato Python stdlib, sem pacote externo;
10. adiciona teste negativo que força `claim_allowed=true` e exige rejeição;
11. adiciona alvos Makefile ARMv7 e AArch64;
12. adiciona matrix CI para cross-build e inspeção ELF sem dependência dinâmica;
13. adiciona `nm -u` como gate contra símbolos externos não resolvidos;
14. remove dependência implícita ARMv7 de `libgcc` com `src/raf_aeabi_uldivmod.S` autoral/freestanding;
15. adiciona política de promoção de evidência e threat model.

## Receipts e fechamento append-only

Run observado: `33126761644`, head `41c938f6faa2bbd0b25c453441c8f1aa7f5491dd`, conclusão `success`.

Receipt sucessor: `receipts/2026-08-27/ci-run-33126761644-v2-cross-isa.v1.json`.

Transições fechadas por evidência:

- `TV-V2-X86-CI: TOKEN_VAZIO_RUNNER_PENDING -> CROSS_HOST_BOUNDED_PASS`;
- `TV-ARMV7-CROSS-BUILD: TOKEN_VAZIO_RUNNER_PENDING -> CROSS_BUILD_VERIFIED`;
- `TV-AARCH64-CROSS-BUILD: TOKEN_VAZIO_RUNNER_PENDING -> CROSS_BUILD_VERIFIED`.

O primeiro cross-build ARMv7 falsificou a hipótese de ausência total de runtime implícito: a divisão decimal de `raf_u64` solicitou `__aeabi_uldivmod`. A correção não adicionou `libgcc`; implementou o helper EABI dentro do repositório. O run sucessor compilou e passou os gates estáticos.

## TOKEN_VAZIO ainda legítimos

- `TV-ARMV7-PHYSICAL-EXECUTION`;
- `TV-AARCH64-PHYSICAL-EXECUTION`;
- `TV-UTF8-VALIDATION`;
- `TV-FULL-DOMAIN-SCHEMA-SEMANTICS`;
- `TV-RAW018-CURRENT-PROVIDER-BYTES-SHA256-JSON-PARSE`;
- `TV-BLAKE3-TOOL_WHERE_UNAVAILABLE`;
- `TOKEN_VAZIO_OWNER_LICENSE_SELECTION`.

### RAW018 — probe adicional 2026-08-27

O Drive confirma por dois planos históricos independentes `path=conversations-018.json` e `size_bytes=12115336`, além do commitment privacy-preserving do conjunto de 100 PIDs. Uma busca direta atual no Drive acessível por `name = 'conversations-018.json'` retornou zero providers. Portanto o gap foi novamente testado, mas **não** pode ser promovido: search miss não prova ausência global e não fornece byte stream/SHA/parse.

F_next RAW018 permanece: localizar provider/materialização atual -> exigir 12.115.336 bytes -> SHA-256 -> parse fail-closed -> comparar commitment PID -> receipt sucessor.

## Não-regressão

Nenhum receipt V1 foi substituído. V2 é sucessor de implementação e contrato; a evidência histórica permanece referenciável pela versão original.

Build cruzado não é execução física. CI verde não é validação de domínio. FNV-1a não é hash criptográfico.

`TOKEN_VAZIO -> evidência -> receipt_sucessor`, nunca `TOKEN_VAZIO -> suposição`.

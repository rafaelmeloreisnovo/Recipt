# EVOLUTION V2 — 2026-08-27

Estado inicial: `V1_IMPLEMENTED / MEASURED_LOCAL_X86_64 / CROSS_HOST_BOUNDED_PASS`

Estado pretendido por esta evolução: `V2_GRAMMAR_HARDENED / CROSS_ISA_BUILD_EVIDENCED_WHEN_CI_PASSES / claim_allowed=false`

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
5. expande testes de 4 para 18 casos positivos/negativos;
6. adiciona schema V2 com propriedades fechadas e invariantes de sucesso constantes;
7. adiciona validador de contrato Python stdlib, sem pacote externo;
8. adiciona teste negativo que força `claim_allowed=true` e exige rejeição;
9. adiciona alvos Makefile ARMv7 e AArch64;
10. adiciona matrix CI para cross-build e inspeção ELF sem dependência dinâmica;
11. adiciona política de promoção de evidência e threat model.

## Gates de promoção

Enquanto a CI desta branch não for observada, os seguintes estados permanecem:

- `TV-V2-X86-CI = TOKEN_VAZIO_RUNNER_PENDING`;
- `TV-ARMV7-CROSS-BUILD = TOKEN_VAZIO_RUNNER_PENDING`;
- `TV-AARCH64-CROSS-BUILD = TOKEN_VAZIO_RUNNER_PENDING`.

Mesmo se os builds cruzados passarem, continuam abertos:

- `TV-ARMV7-PHYSICAL-EXECUTION`;
- `TV-AARCH64-PHYSICAL-EXECUTION`;
- `TV-UTF8-VALIDATION`;
- `TV-FULL-DOMAIN-SCHEMA-SEMANTICS`;
- `TV-RAW018-CURRENT-PROVIDER-BYTES-SHA256-JSON-PARSE`;
- `TOKEN_VAZIO_OWNER_LICENSE_SELECTION`.

## Não-regressão

Nenhum receipt V1 foi substituído. V2 é sucessor de implementação e contrato; a evidência histórica permanece referenciável pela versão original.

`TOKEN_VAZIO -> evidência -> receipt_sucessor`, nunca `TOKEN_VAZIO -> suposição`.

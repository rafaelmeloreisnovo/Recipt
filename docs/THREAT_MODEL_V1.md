# Threat Model V1 — Recipt Freestanding Ω

Estado: `ACTIVE / bounded / fail-closed`

## Ativos protegidos

1. integridade dos receipts;
2. separação entre evidência e claim;
3. privacidade do corpus ingerido;
4. reprodutibilidade do binário e da fonte;
5. cadeia de custódia append-only;
6. limites de arquitetura/host/dispositivo;
7. licença e proveniência sem permissões inventadas.

## Ameaças e controles

| Risco | Falha possível | Controle atual | Residual |
| --- | --- | --- | --- |
| JSON malformado estruturalmente equilibrado | falso PASS do V1 | gate gramatical V2 + 18 fixtures | UTF-8 completo e schema semântico continuam fora do kernel |
| profundidade adversarial | overflow de pilha lógica | limite estático 512 + erro fail-closed | DoS por volume ainda é possível por stdin ilimitado |
| corpus gigante | uso excessivo de memória | buffer fixo 64 KiB, sem heap | tempo/IO cresce com bytes |
| dependência dinâmica acidental | regressão freestanding | `readelf -d`, ausência de NEEDED, `-nostdlib -static` | toolchain continua parte da TCB de build |
| supply-chain do CI | action/toolchain alterado | checkout pinado por SHA; permissões read-only | runner Ubuntu e pacotes apt não são bit-reproducíveis |
| cross-build confundido com runtime | claim inflado | classe `CROSS_BUILD_VERIFIED`; physical execution separado | dispositivo físico ainda TOKEN_VAZIO |
| FNV tratado como hash seguro | falsa integridade | documentação restringe FNV a fingerprint | SHA-256/BLAKE3 ficam no envelope externo |
| vazamento de dados pessoais | valores do corpus em commit/log | kernel emite contagens, não valores; receipts não incluem conteúdo bruto | nomes de arquivos/metadados ainda exigem revisão caso a caso |
| alteração de receipt histórico | perda de custódia | sucessores append-only; V1 preservado | proteção de branch não pôde ser verificada pela integração atual |
| licença presumida | uso jurídico indevido | `LicenseRef-...-PENDING` + sem grant público inventado | seleção autoral da licença continua aberta |
| schema drift | saída e contrato divergem | validador stdlib no CI + schema V2 fechado | validador implementa subconjunto documentado de JSON Schema |
| claim escalation | `claim_allowed=true` indevido | const `false` no schema + teste negativo CI | promoção de domínio deve ocorrer fora do kernel |

## Trust boundaries

`stdin_untrusted -> kernel -> receipt_json -> schema_gate -> custody_envelope -> domain_gate`

O kernel não deve acessar rede, arquivos arbitrários, relógio, secrets ou ambiente para produzir métricas do fluxo. Identidade de arquivo, SHA-256, timestamp e dispositivo pertencem ao envelope externo de custódia.

## TOKEN_VAZIO prioritários

- `TV-ARMV7-PHYSICAL-EXECUTION`;
- `TV-AARCH64-PHYSICAL-EXECUTION`;
- `TV-UTF8-VALIDATION`;
- `TV-FULL-DOMAIN-SCHEMA-SEMANTICS`;
- `TV-RAW018-CURRENT-PROVIDER-BYTES-SHA256-JSON-PARSE`;
- `TV-BLAKE3-TOOL` onde a ferramenta não existe;
- `TOKEN_VAZIO_OWNER_LICENSE_SELECTION`.

Nenhum deles deve ser preenchido por inferência.

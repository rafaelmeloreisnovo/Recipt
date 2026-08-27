# Recipt Freestanding Ω — streaming evidence kernel

Estado de desenvolvimento: `V2_HARDENING / claim_allowed=false`

`Recipt` é um núcleo pequeno e auditável para transformar observações de execução em receipts delimitados. O caso inicial lê `conversations.json` de exportações ChatGPT por `stdin`, sem libc, heap, `malloc`, GC ou dependências de runtime, usando buffer estático de 64 KiB.

## Invariante

`VISÃO != FONTE != BUILD != EXECUÇÃO != EVIDÊNCIA != CLAIM`

Nenhuma camada promove automaticamente a seguinte. `claim_allowed=false` permanece hard-coded no kernel e no schema de receipt.

## V1 preservado

O receipt histórico V1 continua imutável e registra a medição real já realizada em x86_64:

- `conversations.json`: 792.693.581 bytes;
- 2.573 objetos de topo;
- zero erro estrutural;
- 4,19 s no host observado;
- `jq --stream`: mesma contagem de 2.573 em 21,27 s;
- relação local 5,076x, explicitamente sem claim universal de performance.

V1 mede equilíbrio estrutural. Ele não é reescrito retroativamente.

## V2 — defesa em profundidade

V2 mantém o scanner de contagens e acrescenta um validador gramatical streaming independente. O gate adicional rejeita, entre outros:

- vírgula ausente ou trailing comma;
- dois-pontos ausente;
- literais `true/false/null` incompletos;
- números JSON inválidos e zero à esquerda;
- escapes inválidos;
- `\uXXXX` malformado;
- conteúdo extra após o valor raiz.

A saída V2 separa:

- `structural_ok`: equilíbrio estrutural e contrato de array raiz;
- `json_syntax_ok`: gramática JSON aceita;
- `claim_allowed`: sempre `false` neste kernel.

O escopo permanece deliberadamente menor que um parser de domínio: V2 não afirma validação UTF-8 completa, schema semântico do corpus, identidade de conversas, hash criptográfico pelo FNV-1a ou benchmark universal.

## Build e testes

```sh
make
make test
make verify-static
```

`make test` executa fixtures positivas e negativas, gera uma saída V2 e a valida contra `schemas/stream-json-receipt.schema.v2.json` com um validador Python stdlib sem dependências externas.

Execução no membro de um ZIP:

```sh
unzip -p export.zip conversations.json | build/raf_stream_json_receipt
```

## ISA

| ISA | Fonte syscall | Gate de build | Execução física |
| --- | --- | --- | --- |
| x86_64 Linux | `syscall` | medido/CI | medido no host histórico |
| AArch64 Linux/Android | `svc 0` | alvo `make aarch64`; promoção depende de CI observado | `TOKEN_VAZIO_AARCH64_PHYSICAL_EXECUTION` |
| ARMv7 EABI Linux/Android | `svc 0`, `r7` | alvo `make armv7`; promoção depende de CI observado | `TOKEN_VAZIO_ARMV7_PHYSICAL_EXECUTION` |

Build cruzado nunca é descrito como execução cruzada.

## Contratos e risco

- V1 schema histórico: `schemas/stream-json-receipt.schema.json`;
- V2 schema fechado: `schemas/stream-json-receipt.schema.v2.json`;
- política: `docs/EVIDENCE_PROMOTION_POLICY_V1.md`;
- threat model: `docs/THREAT_MODEL_V1.md`;
- auditoria histórica: `docs/AUDIT_CONTINUITY_2026-08-27.md`.

FNV-1a é somente fingerprint operacional. SHA-256/BLAKE3, identidade do artefato, host, timestamp e dispositivo pertencem ao envelope externo de custódia.

## TOKEN_VAZIO não falsificado

Ainda exigem evidência própria:

- execução física ARMv7/AArch64;
- validação UTF-8 completa;
- schema semântico completo do corpus;
- custódia atual RAW018;
- BLAKE3 quando a ferramenta estiver indisponível;
- seleção autoral da licença pública.

Um TOKEN_VAZIO só fecha com receipt sucessor e evidência correspondente.

Assinatura autoral: `RAFCODE-Φ-∆RafaelVerboΩ-𓂀ΔΦΩ`.

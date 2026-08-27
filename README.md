# Recipt Freestanding Ω — streaming evidence kernel

Estado: `IMPLEMENTED / MEASURED_LOCAL_X86_64 / claim_allowed=false`

Este pacote inaugura o repositório `Recipt` com um núcleo pequeno e auditável para ler um fluxo JSON sem libc, heap, `malloc`, GC ou dependências de runtime. O caso inicial é `conversations.json` de uma exportação ChatGPT, recebido por `stdin` e processado em buffer estático de 64 KiB.

## Separações obrigatórias

- `structural_ok=true` significa somente que delimitadores, strings e escapes observados ficaram estruturalmente equilibrados.
- O scanner **não** substitui um parser JSON semântico completo.
- `top_level_objects` conta objetos diretamente dentro do array raiz; no formato esperado, isso corresponde ao número observado de conversas.
- FNV-1a é fingerprint operacional, não hash criptográfico.
- SHA-256/MD5/BLAKE3 pertencem ao manifesto externo de custódia; ausência de ferramenta vira `TOKEN_VAZIO`, nunca digest inventado.
- Build cruzado não é execução cruzada. ARMv7/AArch64 compilado ou inspecionado não vira benchmark de dispositivo.

## Build e teste local

```sh
make
make test
```

Execução no membro de um ZIP:

```sh
unzip -p export.zip conversations.json | build/raf_stream_json_receipt
```

## Alvos

| ISA | Fonte de syscall | Estado neste receipt |
| --- | --- | --- |
| x86_64 Linux | `syscall` | compilado e executado localmente |
| AArch64 Linux/Android | `svc 0` | fonte implementada; build/execução = `TOKEN_VAZIO` sem toolchain/alvo |
| ARMv7 EABI Linux/Android | `svc 0`, `r7` | fonte implementada; build/execução = `TOKEN_VAZIO` sem toolchain/alvo |

## Contrato de saída

O binário emite um objeto conforme `schemas/stream-json-receipt.schema.json`. Toda saída conserva `claim_allowed=false`; promoção exige um contrato de domínio posterior.

## Cadeia de custódia

`VISÃO ≠ FONTE ≠ BUILD ≠ EXECUÇÃO ≠ EVIDÊNCIA ≠ CLAIM`

Assinatura autoral: `RAFCODE-Φ-∆RafaelVerboΩ-𓂀ΔΦΩ`.


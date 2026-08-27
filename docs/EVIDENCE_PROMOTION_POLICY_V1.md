# Evidence Promotion Policy V1

Estado: `ACTIVE_POLICY / claim_allowed=false_by_default`

## Invariante central

`VISÃO != FONTE != BUILD != EXECUÇÃO != EVIDÊNCIA != CLAIM`

Nenhuma etapa herda automaticamente a autoridade da etapa anterior. Promoção exige receipt sucessor, evidência observável e falsificador proporcional ao claim.

## Classes

| Classe | Significado | Pode promover claim? |
| --- | --- | --- |
| SOURCE_OBSERVED | fonte/bytes/ref observados | não |
| BUILD_VERIFIED | compilação reproduzida e artefato identificado | não |
| CROSS_BUILD_VERIFIED | build para outra ISA sem execução | não |
| MEASURED_LOCAL | execução medida em um host identificado | não, salvo claim local explicitamente delimitado |
| CROSS_HOST_BOUNDED_PASS | gate independente em outro host | somente o gate delimitado |
| PHYSICAL_DEVICE_EXECUTION | execução em dispositivo físico identificável | somente propriedades diretamente medidas |
| DOMAIN_VALIDATED | contrato de domínio e falsificadores satisfeitos | elegível, nunca automático |

## TOKEN_VAZIO

`TOKEN_VAZIO` é estado epistemológico, não zero, falso ou ausência descartável.

Todo TOKEN_VAZIO deve carregar:

1. identificador estável;
2. claim que permanece bloqueado;
3. evidência faltante;
4. falsificador ou condição de fechamento;
5. próximo probe verificável;
6. receipt sucessor quando fechado.

Estados históricos não são apagados. O fechamento é append-only por sucessor.

## Gates obrigatórios do kernel

- código freestanding: `-ffreestanding -nostdlib -static`;
- nenhuma entrada `NEEDED` no ELF;
- tamanho bounded do binário no CI;
- testes positivos e negativos;
- schema de saída fail-closed;
- `claim_allowed=false` como constante do receipt de baixo nível;
- hash do binário e das fontes relevantes;
- build cruzado não pode ser descrito como execução cruzada;
- valores privados do corpus não entram no receipt salvo autorização explícita e necessidade demonstrada.

## Promoção V1 -> V2

V1 permanece autoridade histórica para receipts já emitidos.

V2 acrescenta uma segunda camada independente:

`structural_scan -> json_grammar_gate`

O gate V2 valida pontuação, chaves, vírgulas, dois-pontos, strings/escapes Unicode `\uXXXX`, números JSON e literais `true/false/null`. Ele deliberadamente não afirma:

- validação semântica de schema do corpus;
- validade Unicode/UTF-8 completa além da gramática de escapes;
- identidade lógica de conversas;
- integridade criptográfica pelo FNV-1a;
- performance geral;
- execução ARM física.

## Regra de risco

Quando a evidência é insuficiente, reduzir o claim antes de aumentar a confiança declarada.

`claim_scope <= evidence_scope`

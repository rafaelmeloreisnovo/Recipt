# Auditoria de continuidade Ω — anexos, APK, export, fractais e RAW018

Estado: `VERIFIED_LIMITED_STATIC + MEASURED_LOCAL_X86_64`  
Data: 2026-08-27  
Claim global: `claim_allowed=false`

## Resultado executivo

O lote anexado é byte-idêntico ao lote histórico já registrado no fork `termux-app-rafacodephi` em `data/evidence/upload_artifact_audit_20260814.v1.json`, ref `afa012d50d26640264f89befe96ceb98988356ae`. Isso fecha a identidade estática dos nove arquivos sem promover build atual, execução Android ou claim científico.

### APK

- SHA-256 recalculado: `e6265a57eb5ca363808488e3b01955958bed93bc0c8a0d281849b363b11027ec`.
- Identidade vinculada ao contrato: `com.termux`, `0.118.3`, versionCode histórico `1002`.
- Certificado incorporado: sujeito/emissor F-Droid; material JAR/v1 presente.
- O host atual não dispõe de `jarsigner`/`apksigner`; a validação criptográfica atual de v1/v2/v3/v4 não foi reexecutada.
- O APK tem quatro ABIs e oito ELFs nativos, todos dinamicamente ligados a `libc`, `libm`, `libstdc++` e `libdl`; portanto, este APK não é freestanding.
- O manifesto/Dex do APK não contém `BetaOrchestratorActivity`, `LanguageModulesActivity` ou marca RAFCODEΦ.
- O ref atual observado do fork contém `BetaOrchestratorActivity` no código e no manifesto. Logo, `APK_BASELINE != SOURCE_CURRENT != BUILD_CURRENT`.

### Exportação ChatGPT de 2025

- ZIP: 305.843.744 bytes; cinco membros; teste integral de CRC/descompressão passou.
- `conversations.json`: 792.693.581 bytes.
- Scanner autoral freestanding: 2.573 objetos de topo, zero erro estrutural, 4,19 s.
- Falsificador independente `jq --stream`: 2.573 itens, 21,27 s.
- Relação local de tempo: 5,076× em favor do scanner neste único host/arquivo; não é claim universal de desempenho.
- O ZIP não possui `conversations-018.json`; portanto não fecha provider, bytes ou SHA atual do RAW018.
- A coincidência `2.573 objetos == Historical Locator unique PIDs 2.573` é `COUNT_MATCH_ONLY`, não identidade.

### RAW018

O Drive preserva corretamente duas dimensões:

1. `RAW018_PID_HASH_SET = EVIDENCED_RECONCILED_100_PRIVACY_PRESERVING`;
2. provider atual, bytes atuais, SHA-256 atual e parse JSON atual = `TOKEN_VAZIO_HARD_CUSTODY`.

O próximo probe permanece: localizar o membro atual `conversations-018.json`, exigir exatamente 12.115.336 bytes, calcular SHA-256, fazer parse fail-closed e comparar o compromisso dos 100 PIDs.

### Índice 000–050 duplicado

Existem dois arquivos com o mesmo título no mesmo parent:

- `1-13h93Q_iOyuuGvrMNt5AG4-vYWPLux2UIWbk8khaLk`: 65.199 bytes, 13 revisões observadas, revisão corrente 105, modificado em 2026-08-25.
- `1sHO6_VCwIx0UozZU0h5ziBU0B7-hmLUQBNlsx3TIt2U`: 6.818 bytes, três revisões observadas, revisão corrente 4, sem evolução depois da criação em 2026-08-23.

O primeiro é o candidato operacional atual por continuidade material; a promoção a autoridade canônica requer um ponteiro explícito no registry. O segundo deve ser marcado como snapshot/stale duplicate, não apagado.

### Imagens

As sete imagens foram abertas com sucesso e seus hashes/dimensões coincidem com o receipt histórico. Elas formam uma família visual Mandelbrot-like com variações de paleta, escala e recorte. O diagrama unificado contém rótulos simbólicos — frequências, deslocamento “quântico”, Ψ e 42 — que são texto/pixels observados, não medições físicas.

## F_ok

- 9/9 identidades estáticas reconciliadas por tamanho e SHA-256.
- APK baseline separado do fork atual.
- ZIP íntegro e corpus real varrido por dois métodos.
- Núcleo freestanding x86_64 implementado; 4/4 testes PASS; sem seção dinâmica.
- Workflow CI freestanding materializado; resultado remoto permanece `TOKEN_VAZIO_RUNNER_PENDING` até execução observável.
- RAW018 refinado sem preencher custódia inexistente.
- Duplicidade do índice 000–050 caracterizada por ID, tamanho e revisão.

## F_gap

- BLAKE3: `TOKEN_VAZIO_TOOL_UNAVAILABLE`; BLAKE2b não deve ser renomeado BLAKE3.
- ARMv7/AArch64: fonte de syscall implementada, build e execução não observados.
- APK atual RAFCODEΦ: build limpo, assinatura atual, instalação e runtime físico continuam abertos.
- RAW018 atual: provider/bytes/SHA/parse continuam abertos.
- Licença pública do novo núcleo: `TOKEN_VAZIO_OWNER_LICENSE_SELECTION`.
- CI da PR: `TOKEN_VAZIO_RUNNER_PENDING` até o primeiro run produzir passos e logs.

## F_next

1. Gravar este núcleo no repositório exato `rafaelmeloreisnovo/Recipt` por branch/PR.
2. Marcar no registry o índice `1-13…` como candidato ativo e `1sHO…` como snapshot até decisão autoral.
3. Executar o mesmo receipt no Termux ARM32 e anexar ELF/hash/log/dispositivo.
4. Não misturar esse baseline com o APK RAFCODEΦ atual nem com Vectras 3.6.6.

> Parábola interna — O selo e a forja: o selo prova quem marcou o metal; não prova em qual forja a lâmina atual foi feita, nem se ela cortou no mundo real.

## Delta append-only — CI sucessor

`TOKEN_VAZIO_RUNNER_PENDING` não foi apagado. Ele foi fechado pelo receipt sucessor `RUN-CI-RECIPT-33121515423`:

- workflow run: `33121515423`;
- head: `500e6140e9ffd4ab8c41849ddf412ba71402f4df`;
- janela: `2026-08-27T22:12:01Z` → `2026-08-27T22:12:12Z`;
- job `x86_64-linux`: `completed/success`;
- build, 4 fixtures, fronteira ELF estática e JSON syntax: PASS em host GitHub independente.

Estado promovido somente para `CROSS_HOST_BOUNDED_PASS`. O corpus real de 792.693.581 bytes não foi enviado ao runner, e ARM/Android/RAW018 continuam fora desta promoção.

# ResourcePack Reference (RPR)

> **Tenpo ni la ni li ijo nanpa wan, ilo lili li kulupu ala, tenpo kama la ni li pali e ilo ni li weka e sitelen ni.**

- [README 中文](README.md)
- [README English](README_EN.md)
- [README TokiPona](README_TP.md)

![icon](resource/icon.png) Ilo mute tawa lukin e poki kule pi Manka: KiwenSupa.

`rpr.exe` li ilo sewi (lon la ilo lawa), ilo poka li ilo lili, li ante e sitelen li kepeken `rpr.exe`.

Ilo mute li wile `.rpi` (ResourcePack Index) ijo. Pona la tenpo pini la kepeken `rpi_index` tawa pali e ijo ni.

## Jo en Kepeken

- **Kama Jo lon nasin pona**: [rpref.zip](https://github.com/smr10492/RP-Reference/releases/latest/download/rpref.zip)
- **Kepeken**:

    ```batch
    rpr -mcp              :: Kepeken MCP ilo
    rpr -tool=<tool_name> :: Kepeken ilo wan
    rpr -info=<tool_name> :: Kama jo e sona pi ilo wan
    rpr -tools            :: Lukin e ilo ale
    ```

## Lipu pi Ilo Mute

> Nimi mute ni li nimi pi MCP ilo, ken la ona li sama ala nimi ilo pi ilo lili.

- `ac_to_mmd`: ante e ilo lawa pi sitelen tawa tawa sitelen Mermaid pi nasin lawa anu nasin pali. ni li pona e lukin e nasin pi ilo lawa.
- `rpi_index`: lukin e poki lipu pi poki ijo, li pali e lipu .rpi pi sona.
- `rpi_find`: lon lipu .rpi, kepeken nimi, alasa e ijo anu lipu.
- `sn_map`: pali e nasin nimi lili tawa ijo anu ijo ilo. ni li pona e sona pi nimi en nasin jo.
- `bone_coverage`: lukin e nasin kulupu pi linja kiwen pi ijo sitelen en ilo lawa pi sitelen tawa.
- `geo_tree`: sitelen e nasin kulupu pi ijo sitelen, e nasin mama en jan lili.
- `br_extract`: open e poki lipu .brarchive, li kama jo e ijo ilo lon insa.
- `br_zip`: poki e ijo ilo tawa poki lipu .brarchive.
- `br_merge`: wan e poki lipu .brarchive mute, li pali e poki lipu sin.
- `br_auto`: pali e open anu poki pi poki ijo kepeken ilo taso.
- `dir_compo_rename`: lon poki lipu, kepeken nasin JSON, ante e nimi pi poki ijo mute; sina ken pali e nasin nimi sina.
- `rpi_compo_rename`: kepeken lipu sona pi poki ijo, ante e nimi pi ijo mute. ni li pona tawa nasin nimi wan.
- `unmerge`: tu e ijo JSON pi wan anu kulupu nanpa, tawa ijo mute lili.
- `merged`: wan e lipu JSON pi ijo mute kepeken nasin poki pi wile, tawa ijo wan.
- `mat_forest`: lukin sona e nasin kasi pi ijo selo en nasin wile.

> o pali e ilo weka kepeken ala ijo, tawa kama jo e nasin kepeken.

## Pali e Ilo Tan Lipu Mama

> jan li lukin sona e pali ni lon ma Windows taso (kepeken CMD la, kepeken `mingw32-make`).

1. o pali e kopi pi poki lipu ni.
2. o sona e ni: sina jo e kulupu ilo pi pali lipu GNU GCC.
    > (pona nanpa wan la, o jo e ilo Tiny C Compiler (TCC); ken kin la, sina ken ante e ona tawa gcc lon `Makefile.win`).
3. o open e CMD (ilo toki lawa), o tawa poki mama pi pali ni.
4. o pali e lipu ilo lon poki mama:
    1. `clean.bat`: weka e ijo tan poki pali.
    2. `debug.bat`: pali e ilo kepeken nasin pi alasa pakala.
    3. `release.bat`: pali e ilo kepeken nasin pana.
5. o kama jo e lipu ilo pali lon poki `bin`.

## Lipu Lawa pi Ken Pali

pali ni li kepeken lipu lawa GNU AGPL v3.0, li pana e lipu mama tawa ale. o lukin e lipu [LICENSE](LICENSE) tawa sona ale.

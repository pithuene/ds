llist
=====

- Daten müssen unabhängig von den next / prev Zeigern einzeln in einem Array liegen.
  - Sonst wäre Struktur nötig was zu Kollision der anonymen Definitionen führen würde.
  - Dann kann allerdings ein Knoten nicht mehr durch einen einzelnen Zeiger identifiziert werden (der Handle in die beiden parallelen Speicherpools ist nötig)

# Wyjaśnienie: Co to jest "run"?

## Definicja

**Run** to ciągła sekwencja **rosnących kątów** θ w trajektorii.

## Przykład

Jeśli mamy sekwencję kątów:
```
45° → 52° → 58° → 65° → 70° → 30° → 35° → 40° → 45° → 50° → 10° → 15° → 20°
```

To identyfikujemy 3 runy:
- **Run 0**: 45° → 52° → 58° → 65° → 70° (długość: 5)
- **Run 1**: 30° → 35° → 40° → 45° → 50° (długość: 5)  
- **Run 2**: 10° → 15° → 20° (długość: 3)

## Run index (run idx)

**Run idx** to numer porządkowy runa w całej sekwencji danych.

W wydruku:
```
alfa_1 x 8: run idx  563 | kat idx  3536- 3888 (352 katow)
```

Oznacza to:
- Blok alfa_1 składa się z 8 powtórzeń wzorca [6,6,7,6,6,6,7]
- Te 8 powtórzeń zaczynają się od **563-ciego runa** w całej sekwencji
- W kategoriach indeksów kątów to zakres 3536-3888 (352 kąty)

## Wzorce alfa i beta

Wzorce to **sekwencje długości runów**, np:
- `alfa_1 = [6, 6, 7, 6, 6, 6, 7]` - oznacza 7 kolejnych runów o długościach: 6, 6, 7, 6, 6, 6, 7
- `beta_1 = [2, 3, 3, 3, ...]` - oznacza run o długości 2, potem 14 runów o długości 3

## Konwersja run idx → kat idx

Aby znaleźć indeks kąta odpowiadający danemu runowi, sumujemy długości wszystkich poprzednich runów.

Przykład:
```python
runs = [6, 6, 7, 6, 6, 6, 7, ...]

# Run idx 0 zaczyna się od kąta 0
# Run idx 1 zaczyna się od kąta 6 (0 + 6)
# Run idx 2 zaczyna się od kąta 12 (0 + 6 + 6)
# Run idx 3 zaczyna się od kąta 19 (0 + 6 + 6 + 7)
```

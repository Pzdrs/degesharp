# DegeSharp

### Jak se to čte?
**DegeSharp** ([degešárp])

## Použití
**Proměnné**

```
# Celá čísla
-?[0-9]+
# Hex číslo
0x[0-9a-f]+

jakoby x = 10;
jakoby y = 20; 
jakoby z = x + y;
```

**Porovnávání hodnot**
```
# Analogicky jako u Pythonu
x > y
x >= y
x < y
x <= y

# Relace == je nyní "je"
x je y
# Relace != je nyní "neni"
x neni y
```

**Aritmetické operace**
```
# Unární
+x
-y

# Binární
x + y;
x - y;
x * y;
x / y;
```
**Logické výrazy a podmínky**
```
# Logický součin (AND) byl vyměněn za "a"
# Logický součet (OR) byl vyměněn za "nebo"
# Logická negace (NOT) byla vyměněna za "nene"

jakoby kladne_obe = x > 0 a y > 0;
jakoby kladne_aspon_jedno = x > 0 nebo y > 0;
jakoby kladne_nebo_zaporne = (x > 0 a y > 0) nebo (x < 0 a y < 0);
jakoby negace = nene kladne_nebo_zaporne;

cokdyz ([condition]) {
    ...
} [jinak] {

}
```

**Forloop**
```
cokdyz (x*2 je y) {
    loop(jakoby i = 0; i < 10; i = i + 1) {
        povidam(x);
        # Continue statement
        skip;
        # Break statement
        vypadny;
    }
}
```
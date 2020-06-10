# Documentació dels jocs de prova del projecte *spaceships*

Aquesta és la documentació dels jocs de prova. Explica què prova cada un i com configurar-lo.

## Informació general
 
Els jocs de prova es troben al fitxer carpeta tests/jplib.c. Es basen en dues funcions principals *jp_all* i *jp_rank*

**Com uso els jocs de prova?**
Cal incloure una de les dues funcions al codi d'*user.c* en funció del que desitjem:
*jp_all*: Permet efectuar tots els jocs de prova.
*jp_rank(int inici, int final)*: Permet efectuar els jocs de prova des del joc *inici* al joc *final*.

## Descripció dels jocs de prova

**1. Prova de tecles i escenaris:** Aquest test implementa un petit joc per mostrar el correcte funcionament del teclat i les mostres en pantalla. Permet moure una @ (arroba) amb les tecles vi ('h' esquerra, 'j' avall, 'k' amunt i 'l' dreta). 

**2. Prova de buffer de teclat:** Aquest test pretén provar que el buffer circular funciona correctament. Per això demanarà a l'usuari que entri el seguit de tecles q w e r t y u i o p (corresponent a la primera fila de lletres del teclat) i esperarà la sortida o p e r t y u i en la que les dues primeres lletres has sigut sobreescrites pel funcionament inherent del buffer. El tamany per defecte del buffer és de 8 caràcters.

**3. Prova de màxim d'fps:** Aquest test vol demostrar la velocitat màxima que poden assolir els fps. Aquesta es mostrarà per pantalla un cop efectuat el càlcul. El resultat sempre és correcte.

**4. Prova de la garantia al regular els fps sense processos:** El test pretén provar si el sistema és capaç de reduir el nombre d'fps a un valor donat. El resultat serà correcte si el nombre d'fps calculat és igual al valor donat. Aquest nombre pot ser modificat canviant la constant FPS. Per a un test correcte aquest valor no hauria de ser major a 275. 

**5. Prova de la garantia al regular els fps amb processos:** El procés efectuarà el mateix càlcul que el joc anterior però en aquest cas compartirà la cpu amb el nombre de processos especificat per la constant CHILS que definirà l'usuari que vulgui fer el test. El test és vàlid si el nombre d'fps calculat és igual al nombre d'fps que l'usuari vol garantir. 

**6. Prova d'fflush():** El test s'usa per comprovar que la crida a sistems _fflush()_ està buidant correctament el buffer del teclar. Per això omplirem el buffer i després d'fflush() mirarem que estigui buit realment.

**7. Prova sbrk():** El test vol comprovar que es fa una correcta gestió de la memòria dinàmica. Per això es farà primer un _sbrk_ amb un paràmetre negatiu per veure que s'ha tractat correctament el cas en el que l'usuari allibera més memòria de la que ha reservat (cap byte en aquest cas). Posteriorment es fan proves reservant i alliberant espais i comprovant que els punters retornin el valor que toca. 

**8. Test final:** Aquest test pretén posar-ho tot a prova. És idèntic al TEST 1 però hi ha altres processos a la CPU, els FPS estan limitats a 70 i les matrius es reserven i s'alliberen de la memòria dinàmica a cada iteració.

 

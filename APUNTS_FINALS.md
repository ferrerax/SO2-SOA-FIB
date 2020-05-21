# Apunts finals de SO2 (i SOA)
Aquests apunts inclouen tot el temari del QP2020 (el del coronavirus) d'SO2/SOA

## 1. Introducció
**Sistema operatiu:** Capa entre el software i el hardware.

**Gestiona:**

- Processos
- Memòria
- I/O i sistema de fitxers
- Multiprocessadors
 
### 1.1 Boot
 power -> bios -> bootloader -> OS
 **Power:** Reset de tots els dispositius i carrega de la BIOS que està harcodejada a la placa. Cal carregar-la en memòria.
 **BIOS:** Basic Input Output System. Detecta i inicia els dispositius hw. Des d'un dispositiu bootable que s'escull, carrega la partició marcada amb el flag de boot. Aquesta té 512 bytes i conté el bootloader.
 **Bootloader:** Carrega la imatge del kernel a memòria. Per això l'ha de trobar. Un cop carregat l'executa
 **OS kernel:** Inicia el SO. Estructures internes, hw necessari etc.
 
 ## 2. Mecanismes d'entrada al sistema
 Cal separar el codi provilegiat del codi d'usuari. Calen mecanismes per comunicar aquests dos codis. El sistema monopolitza tot l'accés a dispositius i HW i prevé que els usuaris hi accedeixin de manera no controlada.
 
 **Instruccions privilegiades:** Aquelles que només pot usar l'SO. Quan una d'elles és executada, el HW comprova que estigui sent executada per codi de sistema. Si no es genera una excepció. **La quantitat de nivells d'execució que pot haver-hi és definit per l'aquitectura.**
 
 **Crida a sistema:** Mecanisme de l'usuari per demanar recursos al sistema.
 
 _Caldra escalar privilegis dins del sistema_ -> **Interrupcions/excepcions** Aquestes permeten canviar el nivell de l'execució mentre duri el seu codi. Per mitjà de la crida `int` explicada mñes abaix.
 
 Aquests mecanismes són les interrupcions. N'hi ha de tres típus:
  - Excepcions: Síncrones i produides per la CPU després de l'execució d'una instrucció. 
  - Interrupcions HW: Asíncrones i produïdes pel HW.
  - Traps: Interrupcions SW. Són síncrones. Les cridem amb la instrucció `int`.
 
 **IDT:** Instruction Descriptor Table. Taula que gestiona les interrupcions. Té 256 entrades. De la 0 a la 31 hi ha les excepcions, de la 32 a la 47 hi ha les interrupcions HW i de la 48 a la 255 els traps. **El `syscall_handler` està a la posició 0x80**. Quan cridem al trap 0x80, al registre `%eax` hi haurà el numero de la syscall per poder tractar amb  la `syscall_table`. Cada entrada de la taula té una rutina a executar i un nivell de privilegi -> el mínim per accedir a la rutina.
 
 **TSS:** Task state segment. Estructura de dades que guarda informació sobre l'estat del sistema. En principi cada procés n'hauria de tenir una però linux només en té una per mantenir distància amb l'arquitectura. Això és una pijada d'intel. Conté informació com per exemple la base de la pila de sistema.
 
 Petit parèntesi: registres bàsics del sistema i que cal sempre tenir en compte al canviar d'user a sistema:
 ```
 eip: instruction pointer
 cs: code segment. Són conté el segment de memòria de codi que s'està executant. 
 flags: Estat del processador
 esp: stack pointer 
 ss: stack segment. Conté el segment de memòria de dades on es troba la pila.
 ```
 
 _Com gestiona el SO les interrupcions (totes)?_
 
 Gestió en dues fases. El codi de control + la rutina:
 
 **Instrucció `int`:** Accepta com a paràmetre el numero a la taula IDT. Compara permissos i mira guarda els registres a dalt mencionats en l'ordre en el que els he descrit i carrega el valor d'aquests de la TSS per poder accedir al codi de sistema. Aquesta crida és l'entrada al codi de sistema i l'elevació de privilegis.
 
 ```
 Interrupt -> IDT(numero int) -> handler -> routine()
 \________/  \________________________________________/
 (user/hw/CPU)             (sistema)
 ```
 
 **Handler:** Escrit en assabler. Depèn de l'aquitectura. És el punt bàsic d'entrada al sistema, fa la gestió del hw necessaria i crida a la rutina de la interrupció. Té els seguents passos:
  - Save All: Guardar els registres del sistema (tots ells).  
  - EOI: **Només per les interrupcions HW**. Notfica al PIC contoller que s'acabo la interrupt. Es fa aquí per si hi ha un canvi de context en plena interrupció. Seria un problema si no es fes aquí i hi hagués un canvi en mig (veure canvi de context més avall).
  - Crida a la rutina.
  - Restore All: Es restaura els registres que s'han guardat amb el _save all_
  - Retorn. Es fa per mitja de la comanda `iret`.
  
  Exemple de handler d'una interrupció HW:
  
  ```
  ENTRY(clock_handler)
    SAVE_ALL;
    EOI;
    call clock_routine;
    RESTORE_ALL;
    iret;
  ```
  
  Syscall handler:
  ```
  ENTRY(system_call_handler)
    SAVE_ALL
    cmpl $0, %eax
    jl err
    cmpl $MAX_SYSCALL, %eax
    jg err
    call *sys_call_table(, %eax, 0x04)  //taula de syscalls
    jmp fin
err:
    movl $-ENOSYS, %eax
fin:
    movl %eax, 0x18(%esp)
    RESTORE_ALL
    iret
  ```


 ### 2.1 Interrupcions HW
 - No tenen codi d'error
 - Necessiten el EOI per notificar al PIC (programmable interrupt controller) de s'ha acabat la interrupció que estava gestionant.
 
 ### 2.2 Excepcions
 Cal tenir en compte que algunes d'elles guarden un codi d'error sobre de la pila. Sobre de l'eip  quan empilen. després de restaurar el context hw (restore all) el borren.
 
 ### 3.3 Syscalls
 S'usa la instrucció `int 0x80` (a windows s'usa 0x2e) amb el codi de la syscall guardat en `%eax` per cridar-les.
 
 **Paràmetres:** S'usa en aquest ordre `ebx`, `ecx`, `esi`, `edi`, `ebp`, i la pila a partis d'aquí. (en windows `ebx` ens apunta a la regió de memòria on estan).
 
 **Retorn:** `eax` contindrà un valor positiu o un codi d'error en negatiu.
 
 _Cal una forma fàcil perquè l'user pugui interactuar amb elles_ -> ús de wrappers.
 
 **Wrapper:** Funció responsable del pas de paràmetres, identificar la crida a sistema demanada per posar-la a `eax` i cridar al trap 0x80 (`syscall_handler`). Un cop retorni al crida a sistema, retornarà el resultat. **Si hi ha hagut un error, posarà el codi d'error a la variable `ERRNO` i retornarà -1**.
 
 Es complica doncs una mica més el fluxe:
 
 ```
 syscall -> wrapper -> IDT(numero int) -> syscallHandler(eax) -> sysCallTable -> rutina
\_________________/   \_________________________________________________________________/
       (user)                                (sistema)
 ```
 #### 3.3.1 Fast Syscalls
 Per mitja de la crida a `sysenter` i `sysexit`.
 S'usa en comptes de `int` per poder-nos petar comprovacions de permissos de la IDT i fer més senzill el fluxe de dalt accedint directament a la syscall_table. Això ho podem fer perque sempre que executem una syscall sóm usuari que demana recursos al sistema, això és dona per controlar, no fa falta comprovar res.
 
 **SYSENTER_EIP_MST:** Registre que guarda l'adreça de la `syscall_table`. Necessari epr fer el canvi al codi de sistema.
 
 **SYSENTER_ESP:MSR:** Apunta a la base de la TSS. S'usa per poder carregar el punter `esp0` que apunta a la base de la pila de sistema.
 
 **Canvis en el wapper:** Com que no fem cap crida a `int`, coses que feia el HW les haurem de fer a manija. Haurem de fer el `save_all` i el `restore_all` i passar correctament els valors al eip, al esp, etc.
 
## 3. Espai de direccions d'un procés
### 3.1 Generació d'executables
 - **Fase 1:**
   - **Compilació:** D'alt nivell a codi objecte
   - **Montatge:** Creació d'executables a partir de codi objecte i llibreries. La diferència entre l'obkecte i l'executable són les adreces. Les del objecte són relatives a l'inici de l'objecte. L'executable te unes capçaleres amb els segments definits.
 - **Fase 2:**
    - **Carregador:** Carrega l'executable en memòria allà on cal.
 
### 3.2 Espais d'adreces
**Espai de direccions lògic del processador:** Rang de direccions a les que pot accedir un processador. Depèn del bus.
**Espai de direccions lògic del procés:** Espai d'un procés en execució. Són les adreces que usa el processador.
**Espai de direccions físic del procés:** Espai d'adreces de memòria física associades a les adreces lògiques del procés.

_Cal doncs una traducció_ -> **MMU** Memory Management Unit. Unitat de HW per fer la traducció.

### 3.3 MMU (Memory Management Unit)

Unitat encarregada de la traducció d'adreces.
```
CPU -> unitat de segmentació -> unitat de paginació -> memòria física
  @lògica                 @lineal                 @física
       \___________________________________________/
                             MMU
 ```
 Un procés està dividit en segments i un segment està dividit en pàgines. Els segments normalment són _codi_, _dades_, _pila_ i _heap_.
 
 - **Unitat de segmentació:** Usa la  taula de segments que apunta a la base de cada segment. Aquesta taula és única per proces. Aquesta taula s'anomena _GDT_.
 - **Unitat de paginació:** És l'encarregada de fer la paginació. _Com ho fem?_ 
 
      ···Usa la **taula de pàgines** -> una entrada per pàgina i una taula per procés. -> 4 MB de taula per cada procés, és molt gran, tenint en compte que s'ha de guardar en memòria. -> Per alleuregir pes estaa organitzada en un directori
     
      **Directori:** És una taula de taules de pàgines. Ens permet multinivell. D'aquesta manera ens assegurem que només tenim les taules de pàgines que ens calen. EL registre ```cr3``` indicarà en tot moment on es troba aquest directori. Cada entrada del directori és una taula de pàgines. L'entrada al directori serà indexada pels bits de major pes de l'adreça lògica. D'aquesta manera usem bits que abans no usavem (una adreça física té 20 bits i una de lògica en té 32 així que usarem aquests 12 per indicar l'entrada del directori, és a dir, per indicar la taula de pàgines a usar).
     
      **TLB:** Translation Lookaside Buffer. Ens permet tenir una caché de les adreces traduides. Per invalidar-la n'hi ha prou amb canviar el valor del registre ```cr3```.
     
## 4. Gestió de processos.
**procés:** Executable carregat en memòria i en execució. Caracteritzat per:
 
  - L'execució d'una seqüència d'instruccions
  - Un estat actual (registres).
  - Conjunt associat de recursos (disc, pantalla, memòria)

### 4.1 Estructures de dades
**PCB:** Process control block. Estructura del sistema que guarda el context d'execució. Entre d'altres conté:
 
 - Identificador del procés (PID).
 - Estat del procés
 - Recursos del procés (pàgines de memòria, fitxers oberts...)
 - Estadístiques del procés. (les stats de zeos)
 - Informació pel planificador, com el quantum o la prioritat.
 - Context d'execució.
 - Altres que no es mencionen en aquesta assignatura
     Exemple zeos, la realitat és molt més complexa:
     ```C
     struct task_struct {     //PCB
       int PID;			/* Process ID. This MUST be the first field of the struct. */
	      page_table_entry * dir_pages_baseAddr;
	      struct list_head list;
	      unsigned long *kernel_esp;
	      unsigned long quantum; //per la planificacio dels processos
	      struct stats estat;
	      int nice; // 0 => prioritat ; 1 => no pL_HEAP_STARTrioritat
	      void *brk; //per la gestio de la mem dinamiq
     };
    ```
 
 
**Pila de sistema:** Cal una pila de sistema per a cada procés. El cap esp0 de la TSS apunta a la base d'aquetsa pila.

**EL PCB i la pila es guarden en una Union anomenada `task_union`**

**Task Union:** Union del PCB + la pila del sistema. Es fa amb un union!! No s'usa struct simplificar les qüestions d'adreces. D'aquesta manera sabem que el top de la pila és el principi del PCB.
  ```C
  union task_union {
      struct task_struct task;
      unsigned long stack[KERNEL_STACK_SIZE];    /* pila de sistema, per procés */
  };
  ```
  **Les `task_union` s'agrupen en un vector. En linux normal està en memòria dinàmica. En zeos té 10 posicions i s'anmena `task`**

_Com s'organitzen els PCBs?_

Els PCBs s'agrupen en llistes doblement encadenades. En zeos la llista és el camp `list` del PCB:

 - **Ready:** Conté els processos que esperen a que el planificador els dongui pas a la cpu  .
 - **Free:** Conté aquells PCBs lliures preparats per quan un nou procés els necessiti.
 - **Run:** Procés/os en execució. En zeos no existeix. Estar _run_ és igual a no tenir 

### Operacions: 

  

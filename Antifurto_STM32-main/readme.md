# Progetto Antifurto con STM32

## Descrizione
Questo progetto realizza un **sistema antifurto distribuito** basato su **tre board STM32F303 Discovery**, progettato per simulare un impianto completo con:

- **armamento e disarmo tramite tastierino numerico**
- **rilevamento movimento** con sensore **HC-SR04**
- **comunicazione seriale UART/USART** tra le board
- **allarme sonoro** tramite buzzer
- **ridondanza** con una terza board di backup

L’obiettivo è garantire il funzionamento del sistema anche in caso di guasto o assenza della board di allarme principale.

---

## Obiettivo del progetto
Il sistema è stato sviluppato per dimostrare:

- l’integrazione di più periferiche hardware su STM32
- la gestione di una logica di controllo distribuita
- la comunicazione tra nodi embedded tramite UART
- l’implementazione di un meccanismo di **fault detection** e **backup automatico**

---

## Architettura del sistema
Il progetto utilizza **tre board STM32F303 Discovery**, ciascuna con un ruolo specifico.

### Board A — Centrale di controllo
Gestisce la logica principale del sistema:

- lettura del **tastierino numerico**
- gestione del **sensore HC-SR04**
- visualizzazione stato su **LCD 16x2**
- invio messaggi via UART alle altre board
- monitoraggio della board B tramite messaggi periodici di stato
- attivazione della board C in caso di fault della board B

### Board B — Allarme primario
Si occupa dell’allarme principale:

- riceve messaggi da A tramite **UART4**
- attiva il **buzzer** quando riceve il comando di allarme
- invia periodicamente ad A un messaggio di conferma (`tutto_ok` / `ok`)

### Board C — Allarme ridondante
Resta in attesa come backup:

- ascolta i messaggi inviati da A tramite **UART5**
- entra in funzione solo se A rileva un malfunzionamento di B
- attiva il buzzer quando riceve il messaggio di attivazione di emergenza

---

## Componenti hardware utilizzati

- **3x STM32F303 Discovery**
- **Tastierino numerico 4x4**
- **Sensore a ultrasuoni HC-SR04**
- **Display LCD 16x2 (controller HD44780)**
- **Buzzer**
- collegamenti UART tra le board

---

## Funzionamento del sistema

### Armamento
1. L’utente inserisce il codice sul tastierino della **Board A**.
2. Il codice corretto è:

```text
1926#
```

3. Se il sistema è disarmato, A passa allo stato **ARMATO**.
4. Il sensore **HC-SR04** viene attivato.
5. Se viene rilevato un movimento, A invia il comando di **ALARM** alla board B.
6. B attiva il buzzer e invia periodicamente un messaggio di stato ad A.

### Disarmo
1. L’utente reinserisce il codice corretto sulla Board A.
2. Se il sistema è armato, A passa allo stato **DISARMATO**.
3. Il sensore viene disattivato.
4. A invia il comando di **STOP** a B e C.
5. L’allarme sonoro viene spento.

---

## Meccanismo di ridondanza
Per migliorare l’affidabilità del sistema è stato implementato un meccanismo di backup.

- La **Board B** invia un messaggio di stato ogni **5 secondi** ad A.
- La **Board A** controlla che questi messaggi arrivino regolarmente.
- Se A non riceve alcun messaggio da B entro il timeout previsto, considera B in fault.
- In questo caso A invia un messaggio di **WAKE** alla **Board C**.
- La board C si attiva come **allarme ridondante**.

Questo approccio permette al sistema di restare operativo anche in caso di guasto o manomissione della board principale di allarme.

---

## Comunicazione UART
Sono state utilizzate due interfacce UART:

- **UART4**: comunicazione **bidirezionale** tra **Board A** e **Board B**
- **UART5**: comunicazione **unidirezionale** da **Board A** a **Board C**

Questa scelta rende il sistema modulare e consente di separare chiaramente il canale principale da quello di backup.

---

## Tastierino numerico
Il tastierino è collegato alla Board A tramite **8 pin**:

- **4 righe**
- **4 colonne**

La lettura avviene tramite **scansione a matrice**:

- viene attivata una riga per volta
- si leggono le colonne per identificare il tasto premuto

---

## Sensore HC-SR04
Il sensore a ultrasuoni HC-SR04 viene utilizzato per rilevare la presenza di un oggetto o il movimento.

### Principio di funzionamento
- il pin **Trig** riceve un impulso di **10 µs**
- il modulo genera 8 impulsi ultrasonici a **40 kHz**
- il pin **Echo** resta alto finché il segnale riflesso non ritorna
- dalla durata del segnale di ritorno si ricava la distanza

---

## Display LCD 16x2
Il display LCD viene usato per mostrare:

- il codice inserito
- lo stato del sistema
- messaggi come:
  - `ARMATO`
  - `DISARMATO`
  - `ALLARME`

È stato collegato in **modalità parallela a 4 bit**, scelta ritenuta adatta al progetto per semplicità e controllo diretto della periferica.

### Funzioni principali utilizzate
- `LCD_Send_Command(uint8_t cmd)`
- `LCD_Send_Data(uint8_t data)`
- `LCD_Send_String(char *str)`
- `LCD_Set_Cursor(uint8_t row, uint8_t col)`

---

## Gestione dei timer

### Timer su Board B
La Board B usa **TIM2** per inviare automaticamente un messaggio di stato ogni **5 secondi** tramite UART4.

**Parametri principali:**
- Prescaler: `9599`
- Period: `24999`
- Clock source: `Internal Clock`
- Modalità: `Up counting`

### Timer su Board A
La Board A usa **TIM4** per rilevare l’assenza di messaggi da B. Se il timer scade, A considera B non disponibile e attiva C.

**Parametri principali:**
- Prescaler: `9599`
- Period: `44999`
- Clock source: `Internal Clock`
- Modalità: `Up counting`

---

## Test effettuati
Durante lo sviluppo sono stati verificati:

- corretto inserimento del codice tramite tastierino
- visualizzazione dello stato su LCD
- rilevamento del movimento tramite HC-SR04
- attivazione del buzzer sulla Board B
- invio periodico del messaggio `ok` da B ad A
- timeout di A in assenza di messaggi da B
- attivazione corretta della Board C come backup
- corretto spegnimento dell’allarme dopo il disarmo
- integrazione hardware/software di tutti i moduli

Il sistema è stato testato anche in condizioni di disconnessione o blocco di una board, verificando la corretta attivazione della ridondanza.



---

## Possibili sviluppi futuri
- notifica **Wi-Fi** su smartphone
- salvataggio eventi di allarme
- gestione di più sensori
- interfaccia utente più avanzata
- batteria di backup o alimentazione ridondante

---

## Autori
- Francesco Prisco
- Luca De Filippo
- Christian Morelli

Corso: **Architettura e Progetto dei Calcolatori**

---

## Conclusione
Il progetto dimostra la realizzazione di un **sistema antifurto embedded funzionante, modulare e robusto**, capace di coordinare più nodi STM32 tramite UART e di mantenere operativo l’allarme grazie a un meccanismo di ridondanza.


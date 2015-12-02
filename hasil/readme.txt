cara pake file
--------------------

Ini adalah file terkompress tool penyuntik, file yg disuntikkan dan aksesorisnya
Tool penyuntik : lpc_dbe
File yang disuntik : santer.hex
File pendkung penyuntik : Makefile

Prosedure nyuntik
1. Pastikan file lpc_dbe beratribut executable.
   Jika tidak yakin lakukan perintah berikut, masuk dulu ke folder ini, buka terminal
   ketik : chmod +x ./lpc_dbe
2. Nyuntik ketik : make tulis



Teg 15V
- Power Regulator : U2
- Aksesoris : D9, C9, P4

+++++ Tes tegangan 15V
- D9 katoda dengan GND

Teg 3.3V
- Power Regulator : U1
- Aksesoris : D2, C3

+++++ Tes tegangan 3.3V
- C3


Prosesor
- microController : U5
- Aksesoris : R33, R34, C16, R32, R29, 
			  C4, X1, C17,
			  R27, C21, R28 --> filter
			  R31, C12, D23, 
			  R8, R7, D17, R30
- Program : R24, P3,
- Cap Decoupling : C2, C18, C19, C20
- Konektor : P2
- indikator Led1 : D14, R16
- indikator Led2 : D3, R5
- indikator Led3 : D5, R17

>>>>> Suntik  <<<<<<

Teg 3.3V utama
- Power Regulator : U105 AP1509-3.3
- Aksesoris : L3, C222, D12, C123, C228


+++++ Tes 
Tegangan 3.3V utama >> C228
Indikator led1 : nyala/mati/blink/blink cepat
Indikator led2 : nyala/mati/blink/blink cepat


ADC
ADC7 : D1, D4, R3, R4, C7, D18
ADC2 : D19, R12, R13, C11


+++++ Tes 
Indikator led2 : nyala/mati/kedip
PWM R30 : frek ... Hz



Pengkondisi Charger
Charger : D21, D10, D15, D22
		  L1, C10 ---> Filter
		  

ADC6 : D20, C8, R10, R11, D11, D13, K11


LM2575 3.3V
LM2576 5V
M95EB Quectel
MAT14 
MIC29302

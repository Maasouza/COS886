\ Model TU
\ LP format - for model browsing. Use MPS format to capture full model detail.
Maximize
  x_0 + x_1 + x_2 + x_3 + x_4
Subject To
 linha_0: x_0 + y_0 = 1
 linha_1: x_1 + y_1 = 1
 linha_2: x_2 + y_2 = 1
 linha_3: x_3 + y_3 = 1
 linha_4: x_4 + y_4 = 1
 coluna_0: x_0 - y_2 = 0
 coluna_1: x_1 - y_2 = 0
 coluna_2: x_0 - y_3 = 0
 coluna_3: x_1 - y_4 = 0
 coluna_4: x_0 - x_1 = 0
 coluna_5: x_3 - x_4 = 0
 coluna_6: x_1 - y_3 = 0
Bounds
Binaries
 x_0 y_0 x_1 y_1 x_2 y_2 x_3 y_3 x_4 y_4
End

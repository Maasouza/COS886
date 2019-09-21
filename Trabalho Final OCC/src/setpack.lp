\ Model selpacking
\ LP format - for model browsing. Use MPS format to capture full model detail.
Maximize
  x_0 + x_1 + x_2 + x_3 + x_4
Subject To
 c_item_0: x_0 + x_2 <= 1
 c_item_1: x_1 + x_4 <= 1
 c_item_2: x_0 + x_1 + x_2 <= 1
 c_item_3: x_0 <= 1
 c_item_4: x_3 <= 1
 c_item_5: x_1 <= 1
 c_item_6: x_2 + x_3 <= 1
 c_item_7: x_4 <= 1
 c_item_8: x_4 <= 1
 c_item_9: x_3 <= 1
Bounds
Binaries
 x_0 x_1 x_2 x_3 x_4
End

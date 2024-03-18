SELECT 
    cus.cus_id, 
    cus.cus_fname, 
    cus.cus_lname, 
    inv.o_items, 
    inv.o_tax, 
    inv.o_subtotal, 
    inv.o_total, 
    inv.wiz_id,  
    wiz.wiz_fname, 
    wiz.wiz_lname
FROM invoice AS inv
JOIN customer AS cus ON cus.cus_id = inv.cus_id
JOIN wizards AS wiz ON wiz.wiz_id = inv.wiz_id

values(2, 1, 3, 8.97);
values(2, 5, 5, 7.85);
values(2, 4, 1, 300.99);
values(3, 3, 1, 60.89);
values(4, 2, 2, 20.98);
var data = [

];

$("[name$=_MEDDRA_PT_ID").autocomplete({
    source: data,
    select: function (event, ui) {
$(this).val(ui.item.label);
//        $("#1_MEDDRA_PT_ID").val(ui.item.label);
    }
});

alert('test');
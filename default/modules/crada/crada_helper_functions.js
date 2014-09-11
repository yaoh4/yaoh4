function ajax_caller (action, arguments, callback, type) {
	if (type == null) type='GET';
	data = jQuery.extend({'action':action}, arguments)
 
	jQuery.ajax({
        type: type,
        url: "./server",
        data: data,
        dataType: "json",
        success: callback,
        error: display_error
    });
} 

function display_error(response, status, error) {
    var errorMessage = error || response.statusText;
    alert(errorMessage);
}

jQuery.fn.serializeObject = function() {
    var o = {};
    var a = this.serializeArray();
    jQuery.each(a, function() {
        if (o[this.name] !== undefined) {
            if (!o[this.name].push) {
                o[this.name] = [o[this.name]];
            }
            o[this.name].push(this.value || '');
        } else {
            o[this.name] = this.value || '';
        }
    });
    return o;
};
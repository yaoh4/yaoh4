function ajax_caller (action, arguments, callback, type='GET') {
	
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

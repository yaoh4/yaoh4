function ajax_caller (action, arguments, callback, type) {
	if (type == null) type='GET';
    if (callback == null) callback='';
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

function stop_spinner(spinner_id, show_id) {
  //Turn off the spinner
  jQuery('#'+spinner_id).hide();
  //Show next menu
  jQuery("#"+show_id).show();
}

/*
function addMadLib(madlib, demographics) {
    //serarch and replace {} with demographic answers
    //Example search {Agency} replace with FDA
    console.log('madlib before');
    console.log(madlib);
    console.log('demographics');
    console.dir(demographics);
    var search_term;
    var replace_term;
    //Check to make sure a madlib is defined
    if (typeof madlib == 'undefined')
        return;

            $.each(demographics, function(key, val) {
                search_term = "{"+key+"}";
                if(replace_term == "") {
                    replace_term = "<b>["+key+"]</b>";
                };
//              replace_term = "<b>"+val+"</b>";
                madlib = madlib.replace(new RegExp(search_term, "g"), replace_term);
            });


    jQuery.each(demographics, function(key, val) {
        search_term = "{"+key+"}";
        replace_term = val;
        alert(replace_term);
        //Search and replace if length is greater than 0
        if(replace_term.length > 0) {
            madlib = madlib.replace(new RegExp(search_term, "g"), replace_term);
        }
    });

    console.log('madlib after');
    console.log(madlib);

    return madlib;
}
*/

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

function setCookie(cname, cvalue, exdays) {
    var d = new Date();
    d.setTime(d.getTime() + (exdays*24*60*60*1000));
    var expires = "expires="+d.toUTCString();
    document.cookie = cname + "=" + cvalue + "; " + expires;
}

function getCookie(cname) {
    var name = cname + "=";
    var ca = document.cookie.split(';');
    for(var i=0; i<ca.length; i++) {
        var c = ca[i];
        while (c.charAt(0)==' ') c = c.substring(1);
        if (c.indexOf(name) != -1) return c.substring(name.length,c.length);
    }
    return "";
}

function explodeLegalArray(entities, bookmark) {
    //Turn an arry into a legal statement to use in a scentense
    //Example: array("Maryland", "New York", "Virginia")
    // = "Maryland, New York, and Virgina"

    var delimiter = ",";
    var output;

    jQuery.each(entities, function (key, value) {
        if(key == 0) {
            output = "<a href='#' id="+bookmark+"-"+value+">"+value+"</a>";
        } else if(key == entities.length-1) {
                output += " and "+"<a href='#' id="+bookmark+"-"+value+">"+value+"</a>";
        } else {
                output += delimiter+" "+"<a href='#' id="+bookmark+"-"+value+">"+value+"</a>";
        }
    });

    return output;

}

function replaceValidationUI2( form ) {
    // Suppress the default bubbles
    console.log("adding evenet listener for invalid");
    form.addEventListener( "invalid", function( event ) {
        event.preventDefault();
    }, true );

    // Support Safari, iOS Safari, and the Android browser—each of which do not prevent
    // form submissions by default
    console.log("adding Event Listener for submit");
    form.addEventListener( "submit", function( event ) {
        if ( !this.checkValidity() ) {
            event.preventDefault();
        }
    });

    // Add a container to hold error messages
    form.insertAdjacentHTML( "afterbegin", "<ul class='error-messages'></ul>" );

    var submitButton = form.querySelector( "button:not([type=button]), input[type=submit]" );
    submitButton.addEventListener( "click", function( event ) {
        var invalidFields = form.querySelectorAll( ":invalid" ),
            listHtml = "",
            errorMessages = form.querySelector( ".error-messages" ),
            label;

        for ( var i = 0; i < invalidFields.length; i++ ) {
            label = form.querySelector( "label[for=" + invalidFields[ i ].id + "]" );
            listHtml += "<li>" +
                label.innerHTML +
                " " +
                invalidFields[ i ].validationMessage +
                "</li>";
        }

        // Update the list with the new error messages
        errorMessages.innerHTML = listHtml;

        // If there are errors, give focus to the first invalid field and show
        // the error messages container
        if ( invalidFields.length > 0 ) {
            invalidFields[ 0 ].focus();
            errorMessages.style.display = "block";
        }
    });
}

function generic_callback(data) {
//  generic_callback for ajax_caller();
    console.info("generic_callback");
    if(data.status == "Error") {
        console.warn(data.message);
        console.dir(data);
    }

    return;
}

function nl2br(varTest) {
    return varTest.replace(/(\r\n|\n\r|\r|\n)/g, "<br />");
};
function br2nl(varTest){
    return varTest.replace(/<br \/>/g, "\n");
};
function br2sp(varTest){
    return varTest.replace(/<br \/>/g, " ");
};

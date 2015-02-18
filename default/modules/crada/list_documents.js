jQuery(function($) {

$(document).ready(function ($) {
  $("#search-term").on('input', updateDocumentList);
  //
  // Check if a search term exists in the cookie
  //

  var search_term = getCookie("Drupal.visitor.document.searchterm");
  if (search_term.length > 0) {
    // Load the search term
    $("#search-term").val(search_term);
    updateDocumentList();
  }
});

function updateDocumentList() {

  var search_term = $("#search-term").val();
  console.log("search term: "+search_term);
  setCookie("Drupal.visitor.document.searchterm", search_term, 1);
  //
  //Send the searchtem to server to get a list of document_id to show.
  //
  ajax_caller('search_clauses', {'searchterm':btoa(getCookie("Drupal.visitor.document.searchterm"))}, search_clauses_callback);

  //console.log("Auto complete: "+search_term);

}

function search_clauses_callback(data) {

//  console.log("search_clauses_callback");
  console.dir(data);
  var count = 0;
  $('#list-document > tbody  > tr').each(function(index) {
    var target_id = $( this ).attr('id');
//    console.log( index + ": " + target_id );
    if($.inArray(target_id, data.filter) == -1) {
//      console.log(target_id+" is NOT in filter");
      $(this).hide("slow");
    } else {
//      console.log(target_id+" is in filter");
      $(this).removeClass();
      if(count % 2 ==0) {
        $(this).addClass('even');
      } else {
        $(this).addClass('odd');
      }
      $(this).show("show");
      count++;
    }
  });


// Highlight the search input box
  if(data.searchterm.length == 0) {
    $("#search-term").removeClass('searching');
    //$("#9").hide();

  } else {
    $("#search-term").addClass('searching');
    //$("#9").show();
  }


}

});

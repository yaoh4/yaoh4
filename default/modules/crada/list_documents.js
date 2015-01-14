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
    filterDocumentList();
  }
});

function filterDocumentList() {

}

function updateDocumentList() {
  //alert('search');
  var search_term = $("#search-term").val();
  setCookie("Drupal.visitor.document.searchterm", search_term, 1);

  if(search_term.length == 0) {
    $("#search-term").removeClass('searching');
    $("#9").hide();

  } else {
    $("#search-term").addClass('searching');
    $("#9").show();
  }

  console.log("Auto complete");
  console.log($("#search-term").text());
  console.log($("#search-term").val());

}

});

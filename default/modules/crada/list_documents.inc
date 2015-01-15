<?php
require_once 'crada_utilities.inc';


function crada_list_documents() {
  crada_include();

  $module_path = drupal_get_path('module', 'crada');
  drupal_add_js($module_path . '/list_documents.js', array('cache' => false, 'preprocess' => false));
  drupal_add_css('sites/all/libraries/fontawesome/css/font-awesome.css');

  global $user;
  crada_log("CRADA_LIST_DOCUMENTS");

  $query = "select * from  ";
  $query .= "(SELECT doc_owner.name, doc_owner.mail, cdr.document_id, cd.title, cd.status,  ";
  $query .= "unix_timestamp(cd.created) as created, unix_timestamp(cd.updated) as updated, ";
  $query .= "r.name as role_name, cdr.access ";
  $query .= "FROM users u, users_roles ur, crada_document_role cdr , crada_document cd, role r, users doc_owner ";
  $query .= "WHERE u.uid = $user->uid ";
  $query .= "and u.uid = ur.uid and ur.rid = cdr.rid and cdr.access <> '' ";
  $query .= "and cdr.document_id = cd.document_id and cd.is_master = 0 ";
  $query .= "and cd.is_master = 0  and ur.rid = r.rid ";
  $query .= "and cd.uid = doc_owner.uid ";
  $query .= "and cd.uid <> {$user->uid} ";
  $query .= "UNION ";
  $query .= "SELECT u.name, u.mail, cd.document_id, cd.title, cd.status,  ";
  $query .= "unix_timestamp(cd.created) as created, unix_timestamp(cd.updated) as updated, ";
  $query .= "'Owner' as role_name, 'write' as access ";
  $query .= "FROM users u, crada_document cd ";
  $query .= "WHERE cd.uid = {$user->uid} and u.uid = cd.uid and cd.is_master = 0) as t ";
  $query .= "order by created desc; ";

  crada_log("HERE IS THE QUERY");

  crada_log($query);
  $result = db_query($query);

// Add a search input
/*
  <div class="input-group margin-bottom-sm">
          <span class="input-group-addon"><i class="fa fa-envelope-o fa-fw"></i></span>
          <input type="text" placeholder="Email address" class="form-control">
        </div>
*/

$html = '<div class="input-group margin-bottom-sm pull-right">
  <span class="input-group-addon"><i class="fa fa-search fa-fw"></i></span>
  <input id="search-term" class="form-control" type="text" placeholder="Search">
</div>';

  $html .= "<h2 title='Accessable Documents'>CRADA Documents</h2>";
  $html .= '<table class="sticky-enabled table-select-processed tableheader-processed sticky-table">
   <thead>
   <tr>
    <th>Document Title</th>
    <th>Status</th>
    <th>Roles</th>
    <th>Created</th>
    <th>Last Updated</th>
    <th>Document Owner</th>
    <th>Operations</th>
   </tr>
  </thead>
  <tbody>';

  $previous_document_id = 0;
  $count = 0;
  foreach ($result as $key => $record) {

    if($previous_document_id != $record->document_id) {

      $row_class = ($count % 2 == 0 ? 'even' : 'odd');
      $active = ($record->status == 1 ? 'active' : 'archived');
      $created = date("F j, Y", $record->created);

      $updated = ($record->updated == null ? 'never' : date("F j, Y", $record->updated));

      //$updated = dateDiff(time(), $record->updated)+ 'ago';
      $document_roles = getDocumentRoles($query, $record->document_id);
      //crada_log(print_r($document_roles, true));
      $document_read_write = "read";

      $html .= "<tr class='$row_class' id='{$record->document_id}'>";
      $html .= '<td>';
      $html .= "  <a title='Edit or view contract' href='load_document?action=Load&document_id={$record->document_id}&version=current'>";
      $html .= $record->title;
      $html .='  </a>';
      $html .= '</td>';
      $html .= "<td>$active</td>";
      $html .= '<td>';
      $html .= '  <div class="item-list">';
      $html .= '    <ul>';
      $html .= $document_roles['html_roles'];
      $html .= '    </ul>';
      $html .= '  </div>';
      $html .= '</td>';
      $html .= "<td>$created</td>";
      $html .= "<td>$updated</td>";
      $html .= "<td>{$record->name} ({$record->mail})</td>";
      $html .= '<td>';
      $html .= "<a href='load_document?action=Load&document_id={$record->document_id}&version=current'>{$document_roles['access']}</a></td>";
      $html .= '</tr>';
    }
    $previous_document_id = $record->document_id;
    $count++;
  }

   $html .= '</tbody>
          </table>';

  return $html;

}

function getDocumentRoles($query, $document_id) {
  //
  //  Had to requery to search through all the roles for this user.
  // This may have performance problems later down the road.
  //
  $result = db_query($query);

  //Get document roles for this document
  //Determine overall document access read or write
  //crada_log("getDocumnetRoles");

  //crada_log("searching for document id: $document_id");
  //crada_log(json_encode($result, true));

  $document_roles = array();
  $write_access_count = 0;
  $html_roles = "";
  foreach ($result as $key => $record) {
    //crada_log("FOR EACH {$record->document_id} {$record->role_name} {$record->role_name}");
    if($record->document_id == $document_id) {
      if($record->access == 'write') {
        $write_access_count++;
      }
      $html_roles .= "<li>";
      $html_roles .= $record->role_name;
      $html_roles .= "({$record->access})";
      $html_roles .= "</li>";
    }
  }
  $document_roles['access'] = ($write_access_count>0 ? 'edit': 'view');
  $document_roles['html_roles'] = $html_roles;
  //crada_log(print_r($document_roles, true));

  return $document_roles;

}

<?php
require_once 'crada_utilities.inc';

function crada_list_documents() {

  global $user;

  $query = "select * from  ";
  $query .= "(SELECT u.name, u.mail, cdr.document_id, cd.title, cd.status,  ";
  $query .= "unix_timestamp(cd.created) as created, unix_timestamp(cd.updated) as updated, ";
  $query .= "r.name as role_name, cdr.access ";
  $query .= "FROM users u, users_roles ur, crada_document_role cdr , crada_document cd, role r  ";
  $query .= "WHERE u.uid = $user->uid ";
  $query .= "and u.uid = ur.uid and ur.rid = cdr.rid and cdr.access <> '' ";
  $query .= "and cdr.document_id = cd.document_id and cd.is_master = 0 ";
  $query .= "and cd.is_master = 0  and ur.rid = r.rid ";
  $query .= "and cd.uid <> 18 ";
  $query .= "UNION ";
  $query .= "SELECT u.name, u.mail, cd.document_id, cd.title, cd.status,  ";
  $query .= "unix_timestamp(cd.created) as created, unix_timestamp(cd.updated) as updated, ";
  $query .= "'Owner' as role_name, 'write' as access ";
  $query .= "FROM users u, crada_document cd ";
  $query .= "WHERE cd.uid = 18 and u.uid = cd.uid and cd.is_master = 0) as t ";
  $query .= "order by updated";

  crada_log($query);
  $result = db_query($query);

  $html = "<h2>Recent Documents</h2>";
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
  foreach ($result as $key => $record) {

    if($previous_document_id != $record->document_id) {

      $row_class = ($key % 2 == 0 ? 'even' : 'odd');
      $active = ($record->status == 1 ? 'active' : 'archived');
      $created = date("F j, Y", $record->created);

      $updated = ($record->updated == null ? 'never' : date("F j, Y", $record->updated));

      //$updated = dateDiff(time(), $record->updated)+ 'ago';
      $document_roles = getDocumentRoles($result, $record->document_id);
      crada_log(print_r($document_roles, true));
      $document_read_write = "read";

      $html .= "<tr class='$row_class'>";
      $html .= '<td>';
      $html .= "  <a title='View contract' href='load_document?action=Load&document_id={$record->document_id}&version=current'>";
      $html .= $record->title;
      $html .='  </a>';
      $html .= '</td>';
      $html .= "<td>$active</td>";
      $html .= '<td>';
      $html .= '  <div class="item-list">';
      $html .= '    <ul>';
      $html .= "      <li>{$record->role_name}</li>";
      $html .= '    </ul>';
      $html .= '  </div>';
      $html .= '</td>';
      $html .= "<td>$created</td>";
      $html .= "<td>$updated</td>";
      $html .= "<td>{$record->name} ({$record->mail})</td>";
      $html .= '<td>';
      $html .= "<a href='load_document?action=Load&document_id={$record->document_id}&version=current'>view</a></td>";
      $html .= '</tr>';
    }
    $previous_document_id = $record->document_id;
  }

   $html .= '</tbody>
          </table>';

  return $html;

}

function getDocumentRoles($result, $document_id) {
  //Get document roles for this document 
  //Determine overall document access read or write
  crada_log("getDocumnetRoles");

  crada_log("document id "+$document_id);
  crada_log(json_encode($result, true));

  $document_roles = array();
  $write_access_count = 0;
  $html_roles = "";
  foreach ($result as $key => $record) {
    crada_log("FOR EACH "+$record->document_id);
    if($record->document_id = $document_id) {
      if($record->access == 'write') {
        $write_access_count++;
        $html_roles .= "<li>{$record->role_name}</li>";
      }
    }
  }
  $document_roles['access'] = ($write_access_count>0 ? 'write': 'read');
  $document_roles['html_roles'] = $html_roles;
  crada_log(print_r($document_roles,true));

  return $document_roles;

}

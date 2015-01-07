<?php

function crada_list_documents() {
  return "Hello";
}

function crada_search_documents() {

 $html = '<table class="sticky-enabled table-select-processed tableheader-processed sticky-table">
 <thead>
 <tr>
  <th>Filename</th>
  <th>Status</th>
  <th>Roles</th>
  <th>Created</th>
  <th>Last Updated</th>
  <th>Document Owner</th>
  <th>Operations</th>
 </tr>
</thead>
<tbody>


 <tr class="odd">
  <td><a title="View contract" href="load_document?action=Load&document_id=2&version=1">IBM Crada 2015</a></td>
  <td>active</td>
  <td>
    <div class="item-list"><ul><li class="first last">NIH</li>
    </ul>
    </div>
  </td>
  <td>January 5, 2015</td>
  <td>25 min 3 sec ago</td>
  <td>
    <a href="load_document?action=Load&document_id=2&version=1">view</a></td>
 </tr>

 <tr class="even">
  <td><a title="View contract" href="load_document?action=Load&document_id=2&version=1">Contract with Medical Center</a></td>
  <td>active</td>
<td>
  <div class="item-list">
    <ul>
      <li class="first">NIH</li>
      <li class="last">administrator</li>
    </ul>
  </div>
</td>
  <td>December 15, 2014</td>
  <td>25 min 3 sec ago</td>
  <td>
    <a href="load_document?action=Load&document_id=2&version=1">view</a></td>
 </tr>


 <tr class="odd">
  <td><a title="View contract" href="load_document?action=Load&document_id=2&version=1">IBM Crada 2015</a></td>
  <td>active</td>
  <td>
    <div>
    <ul><li>CBIIT</li>
    </ul>
    </div>
  </td>

  <td>September 5, 2014</td>
  <td>2 months 1 days</td>
  
  <td>
    <a href="load_document?action=Load&document_id=2&version=1">edit</a>
  </td>
 </tr>
 </tbody>
</table>';

	return $html;

}
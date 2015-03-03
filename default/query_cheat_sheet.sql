/* Find avalilable valid question, answer, clause for a docuemnt*/
set @document_id = 2;
set @master_document_id = (select master_document_id from crada_document where document_id = @document_id);
select cv.* from crada_valid_clause_group_by_answer cv where document_id = @master_document_id;

/* Find current question, answer, clause for a docuemnt*/
set @document_id = 2;
/*
select cv.* from crada_valid_clause_group_by_answer cv where document_id = @master_document_id;
set @master_document_id = (select master_document_id from crada_document where document_id = @document_id);
*/

/* Most current document */
set @document_id = 2;
SELECT *, document_element_id, max(document_version) 
	FROM drupal.crada_document_element where document_id = @document_id 
	group by document_element_id, document_version;

/* Lookup original text for a document, question, answer*/
set @document_id = 7;
set @question_id = 61;
set @answer_id = 0;
set @master_document_id = (select master_document_id from crada_document where document_id = @document_id);
set @clause_group_id = (select cv.clause_group_id from crada_valid_clause_group_by_answer cv 	where cv.document_id = @master_document_id 	and cv.question_id = @question_id 	and cv.answer_id = @answer_id);
set @clause_id = (select cc.clause_id from crada_clause_group_to_clause cc where cc.document_id=@master_document_id and cc.clause_group_id=@clause_group_id);
SELECT cde.document_element_id, cde.document_element_text FROM drupal.crada_document_element cde where cde.document_element_id = @clause_id and cde.document_id = @master_document_id;

/* Same thing in one select*/
SELECT cde.document_element_text, cde.confidential_annotation, cde.public_annotation 
	FROM drupal.crada_document cd, crada_valid_clause_group_by_answer cv, crada_clause_group_to_clause cc, crada_document_element cde
 	where cd.document_id=7 
	and cv.question_id = 61
	and cv.answer_id = 0
	and cd.master_document_id = cv.document_id
	and cc.document_id = cd.master_document_id 
	and cc.clause_group_id = cv.clause_group_id
	and cde.document_element_id = cc.clause_id 
	and cde.document_id = cd.master_document_id;
	
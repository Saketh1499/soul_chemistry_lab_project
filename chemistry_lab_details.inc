<?php
// soul chemistry lab display completed proposals
function chemistry_lab_completed_proposals_all()
{
	$output = "";
	$query = db_select('soul_chemistry_lab_proposal');
	$query->fields('soul_chemistry_lab_proposal');
	$query->condition('approval_status', 3);
	$query->orderBy('actual_completion_date', 'DESC');
	//$query->condition('is_completed', 1);
	$result = $query->execute();
	
	//var_dump($soul_project_abstract);die;
	if ($result->rowCount() == 0)
	{
		$output .= "Work has been completed for the following chemistry lab. We welcome your contributions." ."<hr>";

	} //$result->rowCount() == 0
	else
	{
		$output .= "Work has been completed for the following chemistry lab. We welcome your contributions." . "<hr>";
		$preference_rows = array();
		$i = 1;
		while ($row = $result->fetchObject())
		{
			$proposal_id = $row->id;
			$query1 = db_select('soul_chemistry_lab_submitted_abstracts_file');
			$query1->fields('soul_chemistry_lab_submitted_abstracts_file');
			$query1->condition('file_approval_status', 1);
			$query1->condition('proposal_id', $proposal_id);
			$soul_project_files = $query1->execute();
			$soul_project_abstract = $soul_project_files->fetchObject();
			$completion_date = date("Y", $row->actual_completion_date);
			$preference_rows[] = array(
				$i,
				l($row->project_title, "chemistry-lab-project/soul-chemistry-lab-run/" . $row->id),
				$row->contributor_name,
				$row->university,
				$completion_date
			);
			$i++;
		} //$row = $result->fetchObject()
		$preference_header = array(
			'No',
			'chemistry lab Project',
			'Contributor Name',
			'University / Institute',
			'Year of Completion'
		);
		$output .= theme('table', array(
			'header' => $preference_header,
			'rows' => $preference_rows
		));
	}
	return $output;
}
// soul chemistry lab display in progress proposals
function chemistry_lab_progress_all()
{
	$page_content = "";
	$query = db_select('soul_chemistry_lab_proposal');
	$query->fields('soul_chemistry_lab_proposal');
	$query->condition('approval_status', 1);
	$query->condition('is_completed', 0);
	$result = $query->execute();
	if ($result->rowCount() == 0)
	{
		$page_content .= "Work is in progress for the following chemistry lab under chemistry lab Project<hr>";
	} //$result->rowCount() == 0
	else
	{
		$page_content .= "Work is in progress for the following chemistry lab under chemistry lab Project<hr>";
		$preference_rows = array();
		$i = 1;
		while ($row = $result->fetchObject())
		{
			$approval_date = date("Y", $row->approval_date);
			$preference_rows[] = array(
				$i,
				$row->project_title,
				$row->contributor_name,
				$row->university,
				$approval_date
			);
			$i++;
		} //$row = $result->fetchObject()
		$preference_header = array(
			'No',
			'chemistry lab Project',
			'Contributor Name',
			'Institute',
			'Year'
		);
		$page_content .= theme('table', array(
			'header' => $preference_header,
			'rows' => $preference_rows
		));
	}
	return $page_content;
}

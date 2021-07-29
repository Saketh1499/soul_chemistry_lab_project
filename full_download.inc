<?php

// $Id$
function chemistry_lab_download_full_project()
{
	global $user;
	$id = arg(3);
	$root_path = chemistry_lab_path();
	//var_dump($root_path);die;
	$query = db_select('soul_chemistry_lab_proposal');
	$query->fields('soul_chemistry_lab_proposal');
	$query->condition('id', $id);
	$chemistry_lab_q = $query->execute();
	$chemistry_lab_data = $chemistry_lab_q->fetchObject();
	$CHEMISTRYLAB_PATH = $chemistry_lab_data->directory_name . '/';
	/* zip filename */
	$zip_filename = $root_path . 'zip-' . time() . '-' . rand(0, 999999) . '.zip';
	/* creating zip archive on the server */
	$zip = new ZipArchive();
	$zip->open($zip_filename, ZipArchive::CREATE);
	$query = db_select('soul_chemistry_lab_proposal');
	$query->fields('soul_chemistry_lab_proposal');
	$query->condition('id', $id);
	$chemistry_lab_udc_q = $query->execute();
	$query = db_select('soul_chemistry_lab_proposal');
	$query->fields('soul_chemistry_lab_proposal');
	$query->condition('id', $id);
	$query = db_select('soul_chemistry_lab_submitted_abstracts_file');
	$query->fields('soul_chemistry_lab_submitted_abstracts_file');
	$query->condition('proposal_id', $id);
	$project_files = $query->execute();
	while ($soul_project_files = $project_files->fetchObject()) {
		$zip->addFile($root_path . $CHEMISTRYLAB_PATH . 'project_files/' . $soul_project_files->filepath, $CHEMISTRYLAB_PATH . str_replace(' ', '_', basename($soul_project_files->filename)));
	}
	$zip_file_count = $zip->numFiles;
	$zip->close();
	if ($zip_file_count > 0) {
		if ($user->uid) {
			/* download zip file */
			header('Content-Type: application/zip');
			header('Content-disposition: attachment; filename="' . str_replace(' ', '_', $chemistry_lab_data->project_title) . '.zip"');
			header('Content-Length: ' . filesize($zip_filename));
			ob_clean();
			readfile($zip_filename);
			unlink($zip_filename);
			/*flush();
			ob_end_flush();
			ob_clean();*/
			
		} //$user->uid
		else {
			header('Content-Type: application/zip');
			header('Content-disposition: attachment; filename="' . str_replace(' ', '_', $chemistry_lab_data->project_title) . '.zip"');
			header('Content-Length: ' . filesize($zip_filename));
			header("Content-Transfer-Encoding: binary");
			header('Expires: 0');
			header('Pragma: no-cache');
			//ob_end_flush();
			ob_clean();
			//flush();
			readfile($zip_filename);
			unlink($zip_filename);
		}
	} //$zip_file_count > 0
	else {
		drupal_set_message("There are no chemistry lab project in this proposal to download", 'error');
		drupal_goto('chemistry-lab-project/full-download/project');
	}
}
function chemistry_lab_download_completed_project()
{
		global $user;
	$id = arg(3);
	$root_path = chemistry_lab_path();
	$query = db_select('soul_chemistry_lab_proposal');
	$query->fields('soul_chemistry_lab_proposal');
	$query->condition('id', $id);
	$chemistry_lab_q = $query->execute();
	$chemistry_lab_data = $chemistry_lab_q->fetchObject();
	$CHEMISTRYLAB_PATH = $chemistry_lab_data->directory_name . '/';
	/* zip filename */
	$zip_filename = $root_path . 'zip-' . time() . '-' . rand(0, 999999) . '.zip';
	/* creating zip archive on the server */
	$zip = new ZipArchive();
	$zip->open($zip_filename, ZipArchive::CREATE);
	$query = db_select('soul_chemistry_lab_proposal');
	$query->fields('soul_chemistry_lab_proposal');
	$query->condition('id', $id);
	$chemistry_lab_udc_q = $query->execute();
	$query = db_select('soul_chemistry_lab_proposal');
	$query->fields('soul_chemistry_lab_proposal');
	$query->condition('id', $id);
	$query = db_select('soul_chemistry_lab_submitted_abstracts_file');
	$query->fields('soul_chemistry_lab_submitted_abstracts_file');
	$query->condition('proposal_id', $id);
	$project_files = $query->execute();
	//var_dump($root_path . $CHEMISTRYLAB_PATH . 'project_files/');die;
	while ($soul_project_files = $project_files->fetchObject()) {
		$zip->addFile($root_path . $CHEMISTRYLAB_PATH . 'project_files/' . $soul_project_files->filepath, $CHEMISTRYLAB_PATH . str_replace(' ', '_', basename($soul_project_files->filename)));
	}
	$zip_file_count = $zip->numFiles;
	$zip->close();
	if ($zip_file_count > 0) {
		if ($user->uid) {
			/* download zip file */
			header('Content-Type: application/zip');
			header('Content-disposition: attachment; filename="' . str_replace(' ', '_', $chemistry_lab_data->project_title) . '.zip"');
			header('Content-Length: ' . filesize($zip_filename));
			//ob_end_flush();
			ob_clean();
			//flush();
			readfile($zip_filename);
			unlink($zip_filename);
		} //$user->uid
		else {
			header('Content-Type: application/zip');
			header('Content-disposition: attachment; filename="' . str_replace(' ', '_', $chemistry_lab_data->project_title) . '.zip"');
			header('Content-Length: ' . filesize($zip_filename));
			header("Content-Transfer-Encoding: binary");
			header('Expires: 0');
			header('Pragma: no-cache');
			//ob_end_flush();
			ob_clean();
			//flush();
			readfile($zip_filename);
			unlink($zip_filename);
		}
	} //$zip_file_count > 0
	else {
		drupal_set_message("There are chemistry lab project in this proposal to download", 'error');
		drupal_goto('chemistry-lab-project/full-download/project');
	}
}
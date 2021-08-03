<?php
function chemistry_lab_run_form($form, &$form_state)
{
	$options_first = _list_of_chemistry_lab();
	$url_chemistry_lab_id = (int) arg(2);
	$chemistry_lab_data = _chemistry_lab_information($url_chemistry_lab_id);
	if ($chemistry_lab_data == 'Not found') {
		$url_chemistry_lab_id = '';
	} //$chemistry_lab_data == 'Not found'
	if (!$url_chemistry_lab_id) {
		$selected = isset($form_state['values']['chemistry_lab']) ? $form_state['values']['chemistry_lab'] : key($options_first);
	} //!$url_chemistry_lab_id
	elseif ($url_chemistry_lab_id == '') {
		$selected = 0;
	} //$url_chemistry_lab_id == ''
	else {
		$selected = $url_chemistry_lab_id;
	}
	$form = array();
	$form['chemistry_lab'] = array(
		'#type' => 'select',
		'#title' => t('Title of the chemistry lab'),
		'#options' => _list_of_chemistry_lab(),
		'#default_value' => $selected,
		'#ajax' => array(
			'callback' => 'chemistry_lab_project_details_callback'
		)
	);
	if (!$url_chemistry_lab_id) {
		$form['chemistry_lab_details'] = array(
			'#type' => 'item',
			'#markup' => '<div id="ajax_chemistry_lab_details"></div>'
		);
		$form['selected_chemistry_lab'] = array(
			'#type' => 'item',
			'#markup' => '<div id="ajax_selected_chemistry_lab"></div>'
		);
	} //!$url_chemistry_lab_id
	else {
		$chemistry_lab_default_value = $url_chemistry_lab_id;
		$form['chemistry_lab_details'] = array(
			'#type' => 'item',
			'#markup' => '<div id="ajax_chemistry_lab_details">' . _chemistry_lab_details($chemistry_lab_default_value) . '</div>'
		);
		$form['selected_chemistry_lab'] = array(
			'#type' => 'item',
			'#markup' => '<div id="ajax_selected_chemistry_lab">' . l('Download Abstract', "chemistry-lab-project/download/abstract-file/" . $chemistry_lab_default_value) . '<br>' . l('Download chemistry lab', 'chemistry-lab-project/full-download/project/' . $chemistry_lab_default_value) . '</div>'
		);
	}
	return $form;
}
function chemistry_lab_project_details_callback($form, $form_state)
{
	$commands = array();
	$chemistry_lab_default_value = $form_state['values']['chemistry_lab'];
	if ($chemistry_lab_default_value != 0) {
		$form['chemistry_lab_details']['#markup'] = _chemistry_lab_details($chemistry_lab_default_value);
		$chemistry_lab_details = _chemistry_lab_information($chemistry_lab_default_value);
		$provider = user_load($chemistry_lab_details->uid);
		if ($chemistry_lab_details->uid > 0) {
			$commands[] = ajax_command_html('#ajax_selected_chemistry_lab',l('Download Abstract', "chemistry-lab-project/download/abstract-file/" . $chemistry_lab_default_value) . '<br>' . l('Download chemistry lab', 'chemistry-lab-project/full-download/project/' . $chemistry_lab_default_value));
		} //$chemistry_lab_details->uid > 0
		else {
			$commands[] = ajax_command_html('#ajax_selected_chemistry_lab', '');
			$commands[] = ajax_command_html('#ajax_selected_chemistry_lab_soul', '');
		}
		$commands[] = ajax_command_html('#ajax_chemistry_lab_details', _chemistry_lab_details($chemistry_lab_default_value));
	} //$chemistry_lab_default_value != 0
	else {
		// $form['lab_experiment_list']['#options'] = _ajax_get_experiment_list();
		//  $commands[] = ajax_command_replace('#ajax_selected_experiment', drupal_render($form['lab_experiment_list']));
		$commands[] = ajax_command_html('#ajax_chemistry_lab_details', '');
		$commands[] = ajax_command_html('#ajax_selected_chemistry_lab', '');
		$commands[] = ajax_command_html('#ajax_selected_chemistry_lab_soul', '');
		$commands[] = ajax_command_data('#ajax_selected_chemistry_lab', 'form_state_value_select', $form_state['values']['chemistry_lab']);
	}
	return array(
		'#type' => 'ajax',
		'#commands' => $commands
	);
}
function bootstrap_table_format($headers, $rows)
{
	$thead = "";
	$tbody = "";
	foreach ($headers as $header) {
		$thead .= "<th>{$header}</th>";
	} //$headers as $header
	foreach ($rows as $row) {
		$tbody .= "<tr>";
		foreach ($row as $data) {
			$tbody .= "<td>{$data}</td>";
		} //$row as $data
		$tbody .= "</tr>";
	} //$rows as $row
	$table = "
            <table class='table table-bordered table-hover' style='margin-left:-140px'>
                <thead>{$thead}</thead>
                <tbody>{$tbody}</tbody>
            </table>
        ";
	return $table;
}
/*****************************************************/
function _list_of_chemistry_lab()
{
	$chemistry_lab_titles = array(
		'0' => 'Please select...'
	);
	//$lab_titles_q = db_query("SELECT * FROM {chemistry_lab_proposal} WHERE solution_display = 1 ORDER BY lab_title ASC");
	$query = db_select('soul_chemistry_lab_proposal');
	$query->fields('soul_chemistry_lab_proposal');
	$query->condition('approval_status', 3);
	$query->orderBy('project_title', 'ASC');
	$chemistry_lab_titles_q = $query->execute();
	while ($chemistry_lab_titles_data = $chemistry_lab_titles_q->fetchObject()) {
		$chemistry_lab_titles[$chemistry_lab_titles_data->id] = $chemistry_lab_titles_data->project_title . ' (Proposed by ' . $chemistry_lab_titles_data->name_title . ' ' . $chemistry_lab_titles_data->contributor_name . ')';
	} //$chemistry_lab_titles_data = $chemistry_lab_titles_q->fetchObject()
	return $chemistry_lab_titles;
}
function _chemistry_lab_information($proposal_id)
{
	$query = db_select('soul_chemistry_lab_proposal');
	$query->fields('soul_chemistry_lab_proposal');
	$query->condition('id', $proposal_id);
	$query->condition('approval_status', 3);
	$chemistry_lab_q = $query->execute();
	$chemistry_lab_data = $chemistry_lab_q->fetchObject();
	if ($chemistry_lab_data) {
		return $chemistry_lab_data;
	} //$chemistry_lab_data
	else {
		return 'Not found';
	}
}
function _chemistry_lab_details($chemistry_lab_default_value)
{
	$chemistry_lab_details = _chemistry_lab_information($chemistry_lab_default_value);
	if ($chemistry_lab_default_value != 0) {
		if($chemistry_lab_details->reference != NULL){
		$url = '~(?:(https?)://([^\s<]+)|(www\.[^\s<]+?\.[^\s<]+))(?<![\.,:])~i';
	        $reference = preg_replace($url, '<a href="$0" target="_blank" title="$0">$0</a>', $chemistry_lab_details->reference);
		}else{
		$reference = 'Not provided';
		}
		$form['chemistry_lab_details']['#markup'] = '<span style="color: rgb(128, 0, 0);"><strong>About the chemistry lab</strong></span></td><td style="width: 35%;"><br />' . '<ul>' . '<li><strong>Proposer Name:</strong> ' . $chemistry_lab_details->name_title . ' ' . $chemistry_lab_details->contributor_name . '</li>' . '<li><strong>Title of the chemistry lab:</strong> ' . '<span style="color: #5D125D;">' .$chemistry_lab_details->project_title. '</li>' . '<li><strong>University:</strong> ' . $chemistry_lab_details->university . '</li>' . '<li>'.'<strong>Reference:</strong> ' . $reference .'</li>'.'</ul>';
		$details = $form['chemistry_lab_details']['#markup'];
		return $details;
	} //$chemistry_lab_default_value != 0
}
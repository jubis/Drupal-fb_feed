<?php
/**
*
* Default implementation for theme_fb_feed_head
*
*/
function theme_fb_feed_head($vars) {
	extract($vars);
	
	$head_content = '<div class="head-name"'.$description_title.'><h3>'. $name .'</h3></div>';

	$head = theme('image',array(
				'path' => 'http://graph.facebook.com/'. $id .'/picture',
				'attributes' => array('class'=>'profile-pic')
			));
	$head .= '<div class="head-right-wrapper">';
	$head .= theme('link',array(
				'path' => FB_URL .$id,
				'text' => $head_content,
				'options' => array(
					'attributes' => array('target'=>'_blank'),
					'html' => TRUE
				)
			));
	//$head .= '<div class="head-members">'. $fans .'</div>';
	$head .= '</div>';
	
	return $head;
}

/**
*
* Default implementation for theme_fb_post
*
* @vars
*   Module passes these vars as render arrays or as flat html => render() is used. 
*/
function theme_fb_post($vars) {
	foreach($vars as $key => $var) {
		//var_dump($var);
		$vars[$key] = render($var);
	}
	extract($vars);
	$output = '<div class="fb-post">';
		$output .= '<div class="fb-field field-image">'. $image .'</div>';
		$output .= '<div class="right-wrapper"><div class="right">';
			$output .= '<div class="fb-field field-title">'. $title .'</div>';
			$output .= '<div class="fb-field field-content">'. $content .'</div>';
			if($actions != '') { $output .= '<div class="fb-field field-actions">'. $actions .'</div>'; }
	$output .= '</div></div></div>';
	return $output;
}

/**
*
*
* Default implementation for theme_fb_post_link
*
*/
function theme_fb_post_link($vars){
	//var_dump($vars);
	$link_picture = '';
	if($vars['picture'] !== NULL) {
		$link_picture = '<div class="field-content-link-picture">';
		$link_picture .= theme('image',array(
			'path' => $vars['picture']
		));
		$link_picture .= '</div>';
	}
	$link_desc = '';
	if($vars['description'] !== NULL) {
		$link_desc = '<div class="field-content-link-desc"><p>'. $vars['description'] .'</p></div>';
	}
	$output = '<div class="field-content-link clearfix">';
	$output .= theme('link',array(
		'path' => $vars['link'],
		'options' => array(
			'attributes' => array('target'=>'_blank'),
			'html' => TRUE
		),
		'text' => $link_picture.
			'<div class="link-right-wrapper">'.
				'<div class="field-content-link-name"><h4>'. $vars['name'] .'</h4></div>'.
				$link_desc.
			'</div>'
	));
	return $output;
}

/**
*
* Default implementation for theme_html_tag_wrapper
*  Like html_tag but can function as a theme_wrapper in render array
*  by testing if inner html isn't given in $value but in $children (as drupal_render() passes it).
*
*/
function theme_html_tag_wrapper($vars){
	//drupal_set_message('html_tag_wrapper is called');
	extract($vars);
	$inner_html = '';
	if($value != NULL) {
		$inner_html = $value;
	}
	else {
		$inner_html = $children;
	}
	$output = theme('html_tag',array(
		'element' => array(
			'#tag' => $tag,
			'#attributes' => $attributes,
			'#value' => $inner_html,
			'#value_prefix' => $value_prefix,
			'#value_suffix' => $value_suffix
		)
	));
	//var_dump($output);
	return $output;
}

function theme_fb_feed_css($vars = array()) {
	return drupal_get_path('module','fb_feed') .'/fb_feed.css';
}
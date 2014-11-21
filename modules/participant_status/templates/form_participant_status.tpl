<br />
<form method="post" name="participant_status" id="participant_status">
<table class="std">
    <tr><th colspan="9">Global Participant Status</th></tr>
    {foreach from=$global_participant_status item=row}
        <tr>
        {foreach from=$row item=value key=name}
                <td>{$name} => {$value}</td>
        {/foreach}
        <tr></tr>
    {/foreach}

    <tr>
        <td nowrap="nowrap">{$form.pscid.label}</td>
        <td nowrap="nowrap">{$pscid}
        </td>
    </tr>
    <tr>
        <td nowrap="nowrap">{$form.entry_staff.label}</td>
        <td nowrap="nowrap">{$form.entry_staff.html}
        </td>
    </tr>
    <tr>
        <td nowrap="nowrap">{$form.data_entry_date.label}</td>
        <td nowrap="nowrap">{$form.data_entry_date.html}
        {if $form.errors.data_entry_date}
            <span class='error'>{$form.errors.data_entry_date}</span>
        {/if}
        </td>
    </tr>
    <tr>
        <td nowrap="nowrap">{$form.participant_status.label}</td>
        <td nowrap="nowrap">{$form.participant_status.html}
        {if $form.errors.participant_status}
            <span class='error'>{$form.errors.participant_status}</span>
        {/if}
        </td>
    </tr>
    <tr>
        <td nowrap="nowrap">{$form.reason_specify_group.label}</td>
        <td nowrap="nowrap">{$form.reason_specify_group.html}
        {if $form.reason_specify_group.error}
            <span class='error'>{$form.reason_specify_group.error}</span>
        {/if}
        </td>
    </tr>
    <tr>
        <td nowrap="nowrap">{$form.withdrawal_reasons.label}</td>
        <td nowrap="nowrap">{$form.withdrawal_reasons.html}
        {if $form.withdrawal_reasons.error}
            <span class='error'>{$form.withdrawal_reasons.error}</span>
        {/if}
        </td>
    </tr>
    <tr>
        <td nowrap="nowrap">{$form.withdrawal_reasons_other_specify_group.label}</td>
        <td nowrap="nowrap">{$form.withdrawal_reasons_other_specify_group.html}
        {if $form.withdrawal_reasons_other_specify_group.error}
            <span class='error'>{$form.withdrawal_reasons_other_specify_group.error}</span>
        {/if}
        </td>
    </tr>


    <tr><th colspan="9">Naproxen Participant Status</th></tr>
    <tr>
        <td nowrap="nowrap">{$form.naproxen_eligibility.label}</td>
        <td nowrap="nowrap">{$form.naproxen_eligibility.html}
        {if $form.errors.naproxen_eligibility}
            <span class='error'>{$form.errors.naproxen_eligibility}</span>
        {/if}
        </td>
    </tr>
    <tr>
        <td nowrap="nowrap">{$form.naproxen_eligibility_reason_specify_group.label}</td>
        <td nowrap="nowrap">{$form.naproxen_eligibility_reason_specify_group.html}
        {if $form.naproxen_eligibility_reason_specify_group.error}
            <span class='error'>{$form.naproxen_eligibility_reason_specify_group.error}</span>
        {/if}
        </td>
    </tr>
    <tr>
        <td nowrap="nowrap">{$form.naproxen_eligibility_status.label}</td>
        <td nowrap="nowrap">{$form.naproxen_eligibility_status.html}
        {if $form.errors.naproxen_eligibility_status}
            <span class='error'>{$form.errors.naproxen_eligibility_status}</span>
        {/if}
        </td>
    </tr>
    <tr>
        <td nowrap="nowrap">{$form.naproxen_excluded_reason_specify_group.label}</td>
        <td nowrap="nowrap">{$form.naproxen_excluded_reason_specify_group.html}
        {if $form.naproxen_excluded_reason_specify_group.error}
            <span class='error'>{$form.naproxen_excluded_reason_specify_group.error}</span>
        {/if}
        </td>
    </tr>
    <tr>
        <td nowrap="nowrap">{$form.naproxen_withdrawal_reasons.label}</td>
        <td nowrap="nowrap">{$form.naproxen_withdrawal_reasons.html}
        {if $form.naproxen_withdrawal_reasons.error}
            <span class='error'>{$form.naproxen_withdrawal_reasons.error}</span>
        {/if}
        </td>
    </tr>
    <tr>
        <td nowrap="nowrap">{$form.naproxen_withdrawal_reasons_other_specify_group.label}</td>
        <td nowrap="nowrap">{$form.naproxen_withdrawal_reasons_other_specify_group.html}
        {if $form.naproxen_withdrawal_reasons_other_specify_group.error}
            <span class='error'>{$form.naproxen_withdrawal_reasons_other_specify_group.error}</span>
        {/if}
        </td>
    </tr>


    <tr><th colspan="9">Probucol Participant Status</th></tr>
    <tr>
        <td nowrap="nowrap">{$form.probucol_eligibility.label}</td>
        <td nowrap="nowrap">{$form.probucol_eligibility.html}
        {if $form.errors.probucol_eligibility}
            <span class='error'>{$form.errors.probucol_eligibility}</span>
        {/if}
        </td>
    </tr>
    <tr>
        <td nowrap="nowrap">{$form.probucol_eligibility_reason_specify_group.label}</td>
        <td nowrap="nowrap">{$form.probucol_eligibility_reason_specify_group.html}
        {if $form.probucol_eligibility_reason_specify_group.error}
            <span class='error'>{$form.probucol_eligibility_reason_specify_group.error}</span>
        {/if}
        </td>
    </tr>
    <tr>
        <td nowrap="nowrap">{$form.probucol_eligibility_status.label}</td>
        <td nowrap="nowrap">{$form.probucol_eligibility_status.html}
        {if $form.errors.probucol_eligibility_status}
            <span class='error'>{$form.errors.probucol_eligibility_status}</span>
        {/if}
        </td>
    </tr>
    <tr>
        <td nowrap="nowrap">{$form.probucol_excluded_reason_specify_group.label}</td>
        <td nowrap="nowrap">{$form.probucol_excluded_reason_specify_group.html}
        {if $form.probucol_excluded_reason_specify_group.error}
            <span class='error'>{$form.probucol_excluded_reason_specify_group.error}</span>
        {/if}
        </td>
    </tr>
    <tr>
        <td nowrap="nowrap">{$form.probucol_withdrawal_reasons.label}</td>
        <td nowrap="nowrap">{$form.probucol_withdrawal_reasons.html}
        {if $form.probucol_withdrawal_reasons.error}
            <span class='error'>{$form.probucol_withdrawal_reasons.error}</span>
        {/if}
        </td>
    </tr>
    <tr>
        <td nowrap="nowrap">{$form.probucol_withdrawal_reasons_other_specify_group.label}</td>
        <td nowrap="nowrap">{$form.probucol_withdrawal_reasons_other_specify_group.html}
        {if $form.probucol_withdrawal_reasons_other_specify_group.error}
            <span class='error'>{$form.probucol_withdrawal_reasons_other_specify_group.error}</span>
        {/if}
        </td>
    </tr>


    <tr><th colspan="9">Participant Status Data Changed</th></tr>
    <tr>
        <td nowrap="nowrap">{$form.data_changed_date.label}</td>
        <td nowrap="nowrap">{$form.data_changed_date.html}
        {if $form.errors.data_changed_date}
            <span class='error'>{$form.errors.data_changed_date}</span>
        {/if}
        </td>
    </tr>





        <tr>
        <td nowrap="nowrap">&nbsp;</td>
                <td nowrap="nowrap" colspan="2">
    {if not $success}
        <input class="button" name="fire_away" value="Save" type="submit" />
        <input class="button" value="Reset" type="reset" />
    {/if}
        <input class="button" onclick="location.href='main.php?test_name=timepoint_list&candID={$candID}'" value="Return to profile" type="button" />
        </td>
        </tr>

<td colspan="2">
<table class="fancytable" width="100%">
    <tr><th colspan="10">Participant Status History</th></tr>
    <tr>
        <td nowrap="nowrap"><b>Global Status</b></td>
        <td nowrap="nowrap"><b>Naproxen Eligibility</b></td>
        <td nowrap="nowrap"><b>Naproxen Eligibility Reason</b></td>
        <td nowrap="nowrap"><b>Naproxen Status</b></td>
        <td nowrap="nowrap"><b>Probucol Eligibility</b></td>
        <td nowrap="nowrap"><b>Probucol Eligibility Reason</b></td>
        <td nowrap="nowrap"><b>Probucol Status</b></td>
        <td nowrap="nowrap"><b>Data Entry Staff</b></td>
        <td nowrap="nowrap"><b>Status Changed Date</b></td>
        <td nowrap="nowrap"><b>Last Data Entry Date</b></td>
    </tr>

    {foreach from=$history_list item=row}
	<tr>
	{foreach from=$row item=value key=name}
		{if $value == 'active'}
			<td>Active</td>
		{elseif $value == 'stop_medication_active'}
                        <td>Stop Medication Active</td>
                {elseif $value == 'withdrawn'}
                        <td>Withdrawn</td>
                {elseif $value == 'excluded'}
                        <td>Excluded</td>
                {elseif $value == 'death'}
                        <td>Death</td>
                {elseif $value == 'completed'}
                        <td>Completed</td>
                {elseif $value == 'stop_medication_completed'}
			<td>Stop Medication Completed</td>
		{else}
			<td>{$value}</td>
		{/if}
	{/foreach}
	<tr></tr>
    {/foreach}
</table>
</td>

</table>
{$form.hidden}
</form>


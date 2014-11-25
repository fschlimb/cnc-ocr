{% import "common_macros.inc.c" as util -%}
{#/****** write all puts for given step/env ******/#}
{% macro render_put(output) %}
    {%- set isarange = [] -%}
    {%- for k in (output.key if output.kind == 'ITEM' else output.tag) -%}
    {%- if k.isRanged -%}
    {% do isarange.append(1) %}
    {%- set idx = "i" ~ loop.index0 -%}
    {% call util.render_indented(loop.index0) %}
    for ( int {{idx}} = {{k.start}}; {{idx}} < {{k.end}}; ++{{idx}} )
    {%- endcall -%}
    {% endif -%}
    {%- endfor -%}
    {% call util.render_indented(isarange|count) %}
    {{output.collName ~ ".put( " ~ output.collName ~ "_key_t( "}}
    {%- for k in (output.key if output.kind == 'ITEM' else output.tag) -%}
    {%- if k.isRanged -%}
    {{"i" ~ loop.index0 ~ (", " if not loop.last)}}
    {%- else -%}
    {{k.expr ~ (", " if not loop.last)}}
    {%- endif -%}
    {%- endfor -%}
    {{" )"}}
    {%- if output.kind == 'ITEM' -%}
    {{", " ~ output.binding ~ "_handle"}}
    {%- if isarange|count > 0 -%}
    [/* TODO */]
    {%- endif -%}
    {%- endif -%}
    {{" );"}}
    {%- endcall %}
{%- endmacro %}

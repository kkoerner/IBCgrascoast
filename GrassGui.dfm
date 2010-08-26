object Form1: TForm1
  Left = 9
  Top = 41
  Width = 888
  Height = 715
  Caption = 'Grassland Model 1.0 '
  Color = clBtnFace
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -11
  Font.Name = 'MS Sans Serif'
  Font.Style = []
  OldCreateOrder = False
  PixelsPerInch = 96
  TextHeight = 13
  object Label1: TLabel
    Left = 64
    Top = 396
    Width = 84
    Height = 13
    Caption = 'grazing probability'
  end
  object Label2: TLabel
    Left = 64
    Top = 368
    Width = 64
    Height = 13
    Caption = 'T Max [years]'
  end
  object Label7: TLabel
    Left = 64
    Top = 425
    Width = 80
    Height = 13
    Caption = 'Above Resource'
  end
  object Label8: TLabel
    Left = 64
    Top = 449
    Width = 81
    Height = 13
    Caption = 'Below Resource '
  end
  object MMLegendView1: TMMLegendView
    Left = 632
    Top = 8
    Width = 121
    Height = 97
    NoEntries = 6
    NoEntriesPerGradient = 6
    RoundDigit = 2
    MMColorGrid = MMColorGrid1
  end
  object LWeek: TLabel
    Left = 856
    Top = 16
    Width = 11
    Height = 20
    Caption = '0'
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clWindowText
    Font.Height = -16
    Font.Name = 'MS Sans Serif'
    Font.Style = [fsBold]
    ParentFont = False
  end
  object Label21: TLabel
    Left = 640
    Top = 280
    Width = 6
    Height = 13
    Caption = '0'
  end
  object Label22: TLabel
    Left = 680
    Top = 280
    Width = 59
    Height = 13
    Caption = 'clonal plants'
  end
  object Label23: TLabel
    Left = 640
    Top = 296
    Width = 6
    Height = 13
    Caption = '0'
  end
  object Label24: TLabel
    Left = 680
    Top = 296
    Width = 32
    Height = 13
    Caption = 'genets'
  end
  object Label29: TLabel
    Left = 640
    Top = 312
    Width = 6
    Height = 13
    Caption = '0'
  end
  object Label30: TLabel
    Left = 680
    Top = 312
    Width = 80
    Height = 13
    Caption = 'covered cells (%)'
  end
  object Label20: TLabel
    Left = 792
    Top = 16
    Width = 42
    Height = 20
    Caption = 'week'
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clWindowText
    Font.Height = -16
    Font.Name = 'MS Sans Serif'
    Font.Style = [fsBold]
    ParentFont = False
  end
  object Label33: TLabel
    Left = 640
    Top = 328
    Width = 6
    Height = 13
    Caption = '0'
  end
  object Label34: TLabel
    Left = 680
    Top = 328
    Width = 142
    Height = 13
    Caption = 'number of Generations (mean)'
  end
  object Label44: TLabel
    Left = 640
    Top = 264
    Width = 6
    Height = 13
    Caption = '0'
  end
  object Label45: TLabel
    Left = 680
    Top = 264
    Width = 80
    Height = 13
    Caption = 'non-clonal plants'
  end
  object LSimFile: TLabel
    Left = 112
    Top = 504
    Width = 3
    Height = 13
  end
  object BRun: TButton
    Left = 8
    Top = 600
    Width = 89
    Height = 25
    Caption = 'Grow Plant'
    TabOrder = 0
    OnClick = BRunClick
  end
  object BExit: TButton
    Left = 240
    Top = 640
    Width = 89
    Height = 25
    Caption = 'Exit'
    TabOrder = 1
    OnClick = BExitClick
  end
  object ETmax: TEdit
    Left = 8
    Top = 360
    Width = 49
    Height = 21
    TabOrder = 2
    Text = 'ETmax'
  end
  object PageControl1: TPageControl
    Left = 0
    Top = 0
    Width = 625
    Height = 345
    ActivePage = TabSheet1
    TabIndex = 0
    TabOrder = 3
    object TabSheet1: TTabSheet
      Caption = 'Grids'
      object MMColorGrid1: TMMColorGrid
        Left = 8
        Top = 16
        Width = 297
        Height = 297
        Xcells = 10
        Ycells = 10
        Legend = 
          '<0>:(255,255,255);[1,81]:"Pft type";<98>:(128,128,128):"dead pla' +
          'nt";<99>:(0,0,0):"central point";<101>:(255,255,0):"clonal plant' +
          '";<102>:(0,0,0):"non-clonal plant";'
        PreviewLegendColorsInGrid = True
        AutoPaintOnCanvas = True
      end
      object Label4: TLabel
        Left = 8
        Top = 0
        Width = 110
        Height = 16
        Caption = 'Aboveground Grid'
        Font.Charset = DEFAULT_CHARSET
        Font.Color = clWindowText
        Font.Height = -13
        Font.Name = 'MS Sans Serif'
        Font.Style = []
        ParentFont = False
      end
      object MMColorGrid2: TMMColorGrid
        Left = 312
        Top = 16
        Width = 297
        Height = 297
        Xcells = 10
        Ycells = 10
        Legend = 
          '<0>:(255,255,255);[1,81]:"type";<98>:(128,128,128):"dead plant";' +
          '<99>:(0,0,0):"central point";'
        PreviewLegendColorsInGrid = True
        AutoPaintOnCanvas = True
      end
      object Label5: TLabel
        Left = 312
        Top = -3
        Width = 107
        Height = 16
        Caption = 'Belowground Grid'
        Font.Charset = DEFAULT_CHARSET
        Font.Color = clWindowText
        Font.Height = -13
        Font.Name = 'MS Sans Serif'
        Font.Style = []
        ParentFont = False
      end
    end
  end
  object RadioGroup1: TRadioGroup
    Left = 184
    Top = 352
    Width = 145
    Height = 137
    Caption = 'Above Competition Type'
    Enabled = False
    ItemIndex = 3
    Items.Strings = (
      'Size Symmetry'
      'Height (1/LMR)'
      'Mass   (shoot mass)'
      'Shoot  (mshoot/LMR)'
      'Complete Asymmetry')
    TabOrder = 4
  end
  object PageControl2: TPageControl
    Left = 352
    Top = 349
    Width = 513
    Height = 321
    ActivePage = Clonality
    TabIndex = 0
    TabOrder = 5
    object Clonality: TTabSheet
      Caption = 'Clonality'
      ImageIndex = 4
      object ChClonality: TChart
        Left = 0
        Top = -4
        Width = 505
        Height = 297
        BackWall.Brush.Color = clWhite
        BackWall.Brush.Style = bsClear
        Title.Text.Strings = (
          '')
        BottomAxis.Title.Caption = 'weeks'
        LeftAxis.Logarithmic = True
        LeftAxis.Title.Caption = 'Biomass'
        Legend.Alignment = laBottom
        View3D = False
        TabOrder = 0
        object LineSeries3: TLineSeries
          Marks.ArrowLength = 8
          Marks.Visible = False
          SeriesColor = clRed
          Title = 'clonal Plants'
          Pointer.InflateMargins = True
          Pointer.Style = psRectangle
          Pointer.Visible = False
          XValues.DateTime = False
          XValues.Name = 'X'
          XValues.Multiplier = 1
          XValues.Order = loAscending
          YValues.DateTime = False
          YValues.Name = 'Y'
          YValues.Multiplier = 1
          YValues.Order = loNone
        end
        object LineSeries4: TLineSeries
          Marks.ArrowLength = 8
          Marks.Visible = False
          SeriesColor = clGreen
          Title = 'non-clonal Plants'
          Pointer.InflateMargins = True
          Pointer.Style = psRectangle
          Pointer.Visible = False
          XValues.DateTime = False
          XValues.Name = 'X'
          XValues.Multiplier = 1
          XValues.Order = loAscending
          YValues.DateTime = False
          YValues.Name = 'Y'
          YValues.Multiplier = 1
          YValues.Order = loNone
        end
      end
    end
    object TabSheet8: TTabSheet
      Caption = 'Number PFTs'
      ImageIndex = 5
      object ChPFTs: TChart
        Left = 0
        Top = 0
        Width = 505
        Height = 297
        BackWall.Brush.Color = clWhite
        BackWall.Brush.Style = bsClear
        Title.Text.Strings = (
          '')
        BottomAxis.Title.Caption = 'week'
        LeftAxis.Automatic = False
        LeftAxis.AutomaticMinimum = False
        LeftAxis.Title.Caption = '#individuals'
        Legend.Alignment = laBottom
        RightAxis.Automatic = False
        RightAxis.AutomaticMinimum = False
        RightAxis.Title.Caption = '#PFT'
        View3D = False
        TabOrder = 0
        object Series73: TLineSeries
          Marks.ArrowLength = 8
          Marks.Visible = False
          PercentFormat = '##0.##,%'
          SeriesColor = clRed
          Title = 'No of non-clonal PFTs'
          VertAxis = aRightAxis
          Pointer.InflateMargins = True
          Pointer.Style = psRectangle
          Pointer.Visible = False
          XValues.DateTime = False
          XValues.Name = 'X'
          XValues.Multiplier = 1
          XValues.Order = loAscending
          YValues.DateTime = False
          YValues.Name = 'Y'
          YValues.Multiplier = 1
          YValues.Order = loNone
        end
        object Series74: TLineSeries
          Marks.ArrowLength = 8
          Marks.Visible = False
          SeriesColor = clGreen
          Title = 'No of Individuals'
          Pointer.InflateMargins = True
          Pointer.Style = psRectangle
          Pointer.Visible = False
          XValues.DateTime = False
          XValues.Name = 'X'
          XValues.Multiplier = 1
          XValues.Order = loAscending
          YValues.DateTime = False
          YValues.Name = 'Y'
          YValues.Multiplier = 1
          YValues.Order = loNone
        end
      end
    end
    object TabSheet7: TTabSheet
      Caption = 'Diversity'
      ImageIndex = 5
      object ChBiomass: TChart
        Left = 8
        Top = 0
        Width = 505
        Height = 297
        BackWall.Brush.Color = clWhite
        BackWall.Brush.Style = bsClear
        Title.Text.Strings = (
          '')
        BottomAxis.Title.Caption = 'weeks'
        LeftAxis.Title.Caption = 'shannon index'
        Legend.Alignment = laBottom
        View3D = False
        TabOrder = 0
        object Series75: TLineSeries
          Marks.ArrowLength = 8
          Marks.Visible = False
          SeriesColor = clRed
          Title = 'Above'
          Pointer.InflateMargins = True
          Pointer.Style = psRectangle
          Pointer.Visible = False
          XValues.DateTime = False
          XValues.Name = 'X'
          XValues.Multiplier = 1
          XValues.Order = loAscending
          YValues.DateTime = False
          YValues.Name = 'Y'
          YValues.Multiplier = 1
          YValues.Order = loNone
        end
        object Series76: TLineSeries
          Marks.ArrowLength = 8
          Marks.Visible = False
          SeriesColor = clGreen
          Title = 'Below'
          Pointer.InflateMargins = True
          Pointer.Style = psRectangle
          Pointer.Visible = False
          XValues.DateTime = False
          XValues.Name = 'X'
          XValues.Multiplier = 1
          XValues.Order = loAscending
          YValues.DateTime = False
          YValues.Name = 'Y'
          YValues.Multiplier = 1
          YValues.Order = loNone
        end
      end
    end
  end
  object ProgressBar1: TProgressBar
    Left = 8
    Top = 640
    Width = 209
    Height = 17
    Min = 0
    Max = 100
    TabOrder = 6
  end
  object EPgraz: TEdit
    Left = 8
    Top = 392
    Width = 49
    Height = 21
    TabOrder = 7
    Text = 'EPgraz'
  end
  object EAres: TEdit
    Left = 8
    Top = 421
    Width = 49
    Height = 21
    TabOrder = 8
    Text = 'EAres'
  end
  object EBres: TEdit
    Left = 8
    Top = 445
    Width = 49
    Height = 21
    TabOrder = 9
    Text = 'EBres'
  end
  object Memo1: TMemo
    Left = 104
    Top = 528
    Width = 225
    Height = 105
    Lines.Strings = (
      'Memo1')
    TabOrder = 10
  end
  object BWeek: TButton
    Left = 8
    Top = 536
    Width = 89
    Height = 25
    Caption = 'OneWeek'
    TabOrder = 11
    OnClick = BWeekClick
  end
  object BYear: TButton
    Left = 8
    Top = 568
    Width = 89
    Height = 25
    Caption = 'OneYear'
    TabOrder = 12
    OnClick = BYearClick
  end
  object BLoadFile: TButton
    Left = 8
    Top = 496
    Width = 89
    Height = 25
    Caption = 'SimFile'
    TabOrder = 13
    OnClick = BLoadFileClick
  end
  object StaticText1: TStaticText
    Left = 640
    Top = 128
    Width = 161
    Height = 41
    AutoSize = False
    Caption = 
      'Grid links: aboveground ZOIs; black-non_clonal; colored-one colo' +
      'r a genet'
    TabOrder = 14
  end
  object StaticText2: TStaticText
    Left = 640
    Top = 184
    Width = 161
    Height = 49
    AutoSize = False
    Caption = 'Grid rechts:  belowground ZOIs; colored: each color a PFT'
    TabOrder = 15
  end
  object ODSimFile: TOpenDialog
    InitialDir = 'Input'
    Title = 'Simulation File'
    Top = 488
  end
end

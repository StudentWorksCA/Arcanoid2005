VERSION 5.00
Begin VB.Form Form1 
   Caption         =   "Form1"
   ClientHeight    =   7110
   ClientLeft      =   2820
   ClientTop       =   2010
   ClientWidth     =   9360
   LinkTopic       =   "Form1"
   ScaleHeight     =   7110
   ScaleWidth      =   9360
   Begin VB.CommandButton Command1 
      Caption         =   "Increase size"
      Height          =   375
      Left            =   840
      TabIndex        =   0
      Top             =   1320
      Width           =   1215
   End
   Begin VB.Label txt4 
      AutoSize        =   -1  'True
      ForeColor       =   &H000000FF&
      Height          =   195
      Left            =   1320
      TabIndex        =   7
      Top             =   1920
      Width           =   45
   End
   Begin VB.Label Label3 
      AutoSize        =   -1  'True
      Caption         =   "What hould be changed:"
      Height          =   195
      Left            =   240
      TabIndex        =   6
      Top             =   960
      Width           =   1770
   End
   Begin VB.Label Label2 
      AutoSize        =   -1  'True
      Caption         =   "Minimum angle"
      Height          =   195
      Left            =   960
      TabIndex        =   5
      Top             =   600
      Width           =   1050
   End
   Begin VB.Label Label1 
      AutoSize        =   -1  'True
      Caption         =   "Current angle:"
      Height          =   195
      Left            =   1080
      TabIndex        =   4
      Top             =   240
      Width           =   990
   End
   Begin VB.Label txt3 
      AutoSize        =   -1  'True
      Caption         =   "Label1"
      Height          =   195
      Left            =   2280
      TabIndex        =   3
      Top             =   960
      Width           =   480
   End
   Begin VB.Label txt2 
      AutoSize        =   -1  'True
      Caption         =   "Label1"
      Height          =   195
      Left            =   2280
      TabIndex        =   2
      Top             =   600
      Width           =   480
   End
   Begin VB.Label txt 
      AutoSize        =   -1  'True
      Caption         =   "Label1"
      Height          =   195
      Left            =   2280
      TabIndex        =   1
      Top             =   240
      Width           =   480
   End
   Begin VB.Line l 
      X1              =   1320
      X2              =   3120
      Y1              =   3360
      Y2              =   4560
   End
   Begin VB.Shape b 
      Height          =   1095
      Left            =   6720
      Top             =   4680
      Width           =   1215
   End
   Begin VB.Shape o 
      Height          =   1095
      Left            =   4080
      Top             =   3120
      Width           =   1815
   End
End
Attribute VB_Name = "Form1"
Attribute VB_GlobalNameSpace = False
Attribute VB_Creatable = False
Attribute VB_PredeclaredId = True
Attribute VB_Exposed = False

Private Sub Command1_Click()
o.Width = o.Width + 100
End Sub

Private Sub Form_MouseMove(Button As Integer, Shift As Integer, X As Single, Y As Single)

b.Left = X - b.Width / 2
b.Top = Y - b.Height / 2
l.X1 = o.Left + o.Width / 2
l.Y1 = o.Top + o.Height / 2

l.X2 = X
l.Y2 = Y

Dim curka

bigx = l.X2 - l.X1
bigy = l.Y1 - l.Y2
Dim q
If bigx >= 0 And bigy >= 0 Then
    curka = (Atn(((l.Y1 - l.Y2) / (l.X2 - l.X1)))) * 180 / 3.141592654
ElseIf bigx < 0 And bigy >= 0 Then
    curka = 180 + (Atn(((l.Y1 - l.Y2) / (l.X2 - l.X1)))) * 180 / 3.141592654
ElseIf bigx < 0 And bigy < 0 Then
    curka = 180 + (Atn(((l.Y1 - l.Y2) / (l.X2 - l.X1)))) * 180 / 3.141592654
ElseIf bigx >= 0 And bigy < 0 Then
    curka = 360 + (Atn(((l.Y1 - l.Y2) / (l.X2 - l.X1)))) * 180 / 3.141592654
End If
txt = curka

Dim ug

ug = Atn((o.Height / 2 + b.Height / 2) / (o.Width / 2 + b.Width / 2)) * 180 / 3.141592654
txt2 = ug

If (curka >= 0 And curka <= ug) Or (curka > 360 - ug And curka < 360) Then
    txt3 = "-x"
ElseIf curka > ug And curka < 180 - ug Then
    txt3 = "-y"
ElseIf curka >= 180 - ug And curka <= 180 + ug Then
    txt3 = "+x"
ElseIf curka > 180 + ug And curka <= 360 - ug Then
    txt3 = "+y"
End If


If Abs(bigx) <= (o.Width + b.Width) / 2 And Abs(bigy) <= (o.Height + b.Height) / 2 Then
    txt4 = "COLLISION DETECTED!"
Else
    txt4 = ""
End If


End Sub


